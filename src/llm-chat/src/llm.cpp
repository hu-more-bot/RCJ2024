#include "llama.h"
#include <common.h>
#include <llm.hpp>

#include <log.hpp>

#define PAINTER "<|ai painter|>"
#define CLIENT "<|client|>"

void cb_log_disable(enum ggml_log_level, const char *, void *) {}

LLM::LLM(const char *model, const char *prompt) {
  // Disable Logging
  llama_log_set(cb_log_disable, NULL);

  // init LLM
  llama_backend_init();
  llama_numa_init(GGML_NUMA_STRATEGY_DISABLED);

  // initialize the model
  llama_model_params model_params = llama_model_default_params();

  model_params.n_gpu_layers = 99;

  if (!(llm.model = llama_load_model_from_file(model, model_params)))
    Log::assert("LLM: Failed to load model");

  // initialize the context
  llama_context_params ctx_params = llama_context_default_params();

  ctx_params.seed = -1;
  ctx_params.n_ctx = 2048;
  ctx_params.n_threads = ctx_params.n_threads_batch = get_num_physical_cores();

  if (!(llm.ctx = llama_new_context_with_model(llm.model, ctx_params)))
    Log::assert("LLM: Failed to create llama_context");

  // Create Batch
  llm.batch = llama_batch_init(512, 0, 1);

  // Process Prompt
  std::vector<llama_token> tokens;
  std::vector<llama_token> tmp;

  tmp = tokenize("<|system|>");
  tmp.push_back(llama_token_nl(llm.model));
  tokens.insert(tokens.end(), tmp.begin(), tmp.end());

  tmp = tokenize(prompt);
  tmp.push_back(llama_token_eos(llm.model));
  tmp.push_back(llama_token_nl(llm.model));
  tokens.insert(tokens.end(), tmp.begin(), tmp.end());

  decode(tokens);

  // TODO: may need
  // llama_batch_clear(llm.batch);
}

LLM::~LLM() {
  llama_batch_free(llm.batch);

  llama_free(llm.ctx);
  llama_free_model(llm.model);

  llama_backend_free();
}

std::string LLM::reply(std::string prompt) {
  /* Get User Input */ {
    std::vector<llama_token> tokens;
    std::vector<llama_token> tmp;

    tmp = tokenize(CLIENT);
    tmp.push_back(llama_token_nl(llm.model));
    tokens.insert(tokens.end(), tmp.begin(), tmp.end());

    tmp = tokenize(prompt);
    tmp.push_back(llama_token_eos(llm.model));
    tmp.push_back(llama_token_nl(llm.model));
    tokens.insert(tokens.end(), tmp.begin(), tmp.end());

    tmp = tokenize(PAINTER);
    tmp.push_back(llama_token_nl(llm.model));
    tokens.insert(tokens.end(), tmp.begin(), tmp.end());

    decode(tokens);
  }

  std::string out;

  int n_len = llm.n_cur + 64;
  while (true) {
    // sample the next token
    {
      auto n_vocab = llama_n_vocab(llm.model);
      auto *logits = llama_get_logits_ith(llm.ctx, llm.batch.n_tokens - 1);

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
      if (new_token_id == llama_token_eos(llm.model) || llm.n_cur == n_len) {
        out += "\n";
        // printf("\n");
        break;
      }

      std::string token = llama_token_to_piece(llm.ctx, new_token_id);
      if (token == "<")
        break;

      out += token;
      // printf("%s", token.c_str());

      fflush(stdout);

      // prepare the next batch
      llama_batch_clear(llm.batch);

      // push this new token for next evaluation
      llama_batch_add(llm.batch, new_token_id, llm.n_cur, {0}, true);
    }

    llm.n_cur += 1;

    // evaluate the current batch with the transformer model
    if (llama_decode(llm.ctx, llm.batch))
      Log::assert("LLM: Failed to eval");
  }

  llama_batch_clear(llm.batch);

  return out;
}

std::vector<llama_token> LLM::tokenize(std::string text) {
  return ::llama_tokenize(llm.ctx, text, true);
}

bool LLM::decode(std::vector<llama_token> tokens) {
  for (size_t i = 0; i < tokens.size(); i++)
    llama_batch_add(llm.batch,
                    tokens[i] == '\0' ? llama_token_eos(llm.model) : tokens[i],
                    llm.n_cur + i, {0}, false);

  llm.n_cur += tokens.size();

  llm.batch.logits[llm.batch.n_tokens - 1] = true;

  if (llama_decode(llm.ctx, llm.batch) != 0) {
    fprintf(stderr, "%s: llama_decode() failed\n", __func__);
    return true;
  }

  llm.batch.logits[llm.batch.n_tokens - 1] = false;

  return false;
}

bool LLM::decode(std::string text) {
  std::vector<llama_token> tokens = tokenize(text);
  return decode(tokens);
}