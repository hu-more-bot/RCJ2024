#include <llm.hpp>
#include <common.h>

#include <stdexcept>

LLM::LLM(const char *model, const char *prompt)
{
    printf("Loading LLM...\n");

    const int n_len = 32;

    // init LLM
    llama_backend_init();
    llama_numa_init(GGML_NUMA_STRATEGY_DISABLED);

    // initialize the model
    llama_model_params model_params = llama_model_default_params();

    // model_params.n_gpu_layers = 99; // offload all layers to the GPU
    llm.model = llama_load_model_from_file(model, model_params);

    if (model == NULL)
        throw std::runtime_error(std::string(__func__) + ": unable to load model");

    // initialize the context
    llama_context_params ctx_params = llama_context_default_params();

    ctx_params.seed = -1;
    ctx_params.n_ctx = 2048;
    ctx_params.n_threads = ctx_params.n_threads_batch = get_num_physical_cores();

    llm.ctx = llama_new_context_with_model(llm.model, ctx_params);

    if (llm.ctx == NULL)
        throw std::runtime_error(std::string(__func__) + ": failed to create the llama_context");

    // Create Batch
    llm.batch = llama_batch_init(512, 0, 1);

    // Process Prompt
    decode("<|system|>\n" + std::string(prompt) + "\n");

    // llama_batch_clear(batch);

    printf("\n<|system|>\n%s\n", prompt);
}

LLM::~LLM()
{
    llama_batch_free(llm.batch);

    llama_free(llm.ctx);
    llama_free_model(llm.model);

    llama_backend_free();
}

std::string LLM::reply(std::string prompt)
{
    // Get User Input
    printf("<|user|> (n_cur=%i)\n%s\n<|ai painter|>\n", llm.n_cur, prompt.c_str());
    decode("<|user|>\n" + prompt + "\n" + "<|ai painter|>\n");

    std::string out;

    while (true)
    {
        // sample the next token
        {
            auto n_vocab = llama_n_vocab(llm.model);
            auto *logits = llama_get_logits_ith(llm.ctx, llm.batch.n_tokens - 1);

            std::vector<llama_token_data> candidates;
            candidates.reserve(n_vocab);

            for (llama_token token_id = 0; token_id < n_vocab; token_id++)
                candidates.emplace_back(llama_token_data{token_id, logits[token_id], 0.0f});

            llama_token_data_array candidates_p = {candidates.data(),
                                                   candidates.size(), false};

            // sample the most likely token
            const llama_token new_token_id =
                llama_sample_token_greedy(llm.ctx, &candidates_p);

            // is it an end of stream?
            if (new_token_id == llama_token_eos(llm.model)) // || n_cur == n_len)
            {
                printf("\n");
                break;
            }

            out += llama_token_to_piece(llm.ctx, new_token_id);
            printf("%s", llama_token_to_piece(llm.ctx, new_token_id).c_str());
            fflush(stdout);

            // prepare the next batch
            llama_batch_clear(llm.batch);

            // push this new token for next evaluation
            llama_batch_add(llm.batch, new_token_id, llm.n_cur, {0}, true);
        }

        llm.n_cur += 1;

        // evaluate the current batch with the transformer model
        if (llama_decode(llm.ctx, llm.batch))
            throw std::runtime_error(std::string(__func__) + ": failed to eval");
    }

    llama_batch_clear(llm.batch);

    return out;
}

bool LLM::decode(std::string text)
{
    std::vector<llama_token> tokens = ::llama_tokenize(llm.ctx, text, true);

    for (size_t i = 0; i < tokens.size(); i++)
        llama_batch_add(llm.batch, tokens[i], llm.n_cur + i, {0}, false);

    llm.n_cur += tokens.size();

    llm.batch.logits[llm.batch.n_tokens - 1] = true;

    if (llama_decode(llm.ctx, llm.batch) != 0)
    {
        fprintf(stderr, "%s: llama_decode() failed\n", __func__);
        return true;
    }

    llm.batch.logits[llm.batch.n_tokens - 1] = false;

    return false;
}