#include <llm.hpp>

#include <llama/common.h>

#include <stdexcept>

void cb_log_disable(enum ggml_log_level, const char *, void *) {}

LLM::LLM(const char *modelPath, const char *promptPath)
{
    // Disable Logging
    llama_log_set(cb_log_disable, NULL);

    /* Load LLM */ {
        // init LLM
        llama_backend_init();
        llama_numa_init(GGML_NUMA_STRATEGY_DISABLED);

        // initialize the model
        llama_model_params model_params = llama_model_default_params();

        if (!(llm.model = llama_load_model_from_file(modelPath, model_params)))
            throw std::runtime_error("LLM(): failed to load model");

        // initialize the context
        llama_context_params ctx_params = llama_context_default_params();

        ctx_params.seed = rand();
        ctx_params.n_ctx = 2048;
        ctx_params.n_threads = ctx_params.n_threads_batch =
            get_num_physical_cores();

        if (!(llm.ctx = llama_new_context_with_model(llm.model, ctx_params)))
            throw std::runtime_error(
                "LLM(): failed to create the llama_context");
    }

    // Create Batch
    llm.batch = llama_batch_init(512, 0, 1);

    char *prompt = NULL;
    /* Load Prompt */ {
        // Open File
        FILE *f = NULL;
        if (!(f = fopen(promptPath, "r")))
            throw std::runtime_error("LLM(): failed to open prompt file");

        // Get File Size
        fseek(f, 0, SEEK_END);
        size_t size = ftell(f);
        fseek(f, 0, SEEK_SET);

        // Read File
        prompt = (char *)malloc(sizeof(char) * size);

        if (!fread(prompt, size, 1, f))
            throw std::runtime_error("LLM(): failed to read prompt");
    }

    // Decode Prompt
    decode(tokenize("<|system|>\n" + std::string(prompt) + "\n"));

    // TODO: may need
    // llama_batch_clear(llm.batch);
}

LLM::~LLM()
{
    llama_batch_free(llm.batch);

    llama_free(llm.ctx);
    llama_free_model(llm.model);

    llama_backend_free();
}

bool LLM::decode(std::string author, std::string text)
{
    return decode(tokenize("<|" + author + "|>\n" + text + "\n"));
}

std::string LLM::generate(std::function<void(std::string)> onNewToken)
{
    std::string out;

    decode(tokenize("<|" + config.name + "|>\n"));

    int n_len = llm.n_cur + 64;
    while (true)
    {
        // sample the next token
        {
            auto n_vocab = llama_n_vocab(llm.model);
            auto *logits =
                llama_get_logits_ith(llm.ctx, llm.batch.n_tokens - 1);

            std::vector<llama_token_data> candidates;
            candidates.reserve(n_vocab);

            for (llama_token token_id = 0; token_id < n_vocab; token_id++)
                candidates.emplace_back(
                    llama_token_data{token_id, logits[token_id], 0.0f});

            llama_token_data_array candidates_p = {candidates.data(),
                                                   candidates.size(), false};

            // sample the most likely token
            const llama_token new_token_id =
                llama_sample_token_greedy(llm.ctx, &candidates_p);

            // is it an end of stream?
            if (new_token_id == llama_token_eos(llm.model))
                //  || llm.n_cur == n_len)
                break;

            char token_raw[16];
            size_t len = llama_token_to_piece(llm.model, new_token_id, token_raw, sizeof(token_raw));

            if (len == 0)
                continue;

            std::string token(token_raw, len);

            if (token == "<" || token == " <" || token == "<|" ||
                token == " <|")
                break;

            out += token;

            onNewToken(token); // call callback

            // prepare the next batch
            llama_batch_clear(llm.batch);

            // push this new token for next evaluation
            llama_batch_add(llm.batch, new_token_id, llm.n_cur, {0}, true);
        }

        llm.n_cur += 1;

        // evaluate the current batch with the transformer model
        if (llama_decode(llm.ctx, llm.batch))
            throw std::runtime_error(std::string(__func__) +
                                     ": failed to eval");
    }

    // llama_batch_clear(llm.batch);

    return out;
}

std::string LLM::reply(std::string author, std::string prompt,
                       std::function<void(std::string)> onNewToken)
{
    decode(author, prompt);
    return generate();
}

bool LLM::save(const char *path)
{
    std::vector<uint8_t> state_mem(llama_get_state_size(llm.ctx));
    const size_t written = llama_copy_state_data(llm.ctx, state_mem.data());

    FILE *f;
    if (!(f = fopen(path, "wb")))
    {
        printf("LLM: failed to create save file\n");
        return false;
    }

    if (!fwrite(&llm.n_cur, sizeof(llm.n_cur), 1, f))
    {
        printf("LLM: failed to save cursor\n");
        return false;
    }

    int fout = fwrite(state_mem.data(), 1, written, f);

    fclose(f);

    if (fout != written)
    {
        printf("LLM: failed to write save data\n");
        return false;
    }
    return true;
}

bool LLM::load(const char *path)
{
    std::vector<uint8_t> state_mem(llama_get_state_size(llm.ctx));

    FILE *f;
    if (!(f = fopen(path, "rb")))
    {
        printf("LLM: failed to load file\n");
        return false;
    }

    if (!fread(&llm.n_cur, sizeof(llm.n_cur), 1, f))
    {
        printf("LLM: failed to load cursor\n");
        fclose(f);
        return false;
    }

    const size_t read = fread(state_mem.data(), 1, state_mem.size(), f);
    fclose(f);

    if (read != llama_set_state_data(llm.ctx, state_mem.data()))
    {
        printf("LLM: failed to set state memory\n");
        return false;
    }

    return true;
}

std::vector<llama_token> LLM::tokenize(std::string text)
{
    return ::llama_tokenize(llm.ctx, text, true);
}

bool LLM::decode(std::vector<llama_token> tokens)
{
    llama_batch_clear(llm.batch);

    const size_t size = tokens.size();
    for (size_t i = 0; i < size; i++)
        llama_batch_add(llm.batch, tokens[i], llm.n_cur + i, {0},
                        i == size - 1);

    llm.n_cur += tokens.size();

    if (llama_decode(llm.ctx, llm.batch) != 0)
    {
        fprintf(stderr, "%s: llama_decode() failed\n", __func__);
        return true;
    }

    return false;
}