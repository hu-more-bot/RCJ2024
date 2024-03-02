#include "common/common.h"
#include "llama.h"

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

const char *path = "/home/samthedev/models/zephyr_q4.gguf";
const char *prompt =
    "You are an AI painter who paints portraits on request on spot in any "
    "style."
    //  "If no details are specified, you surprise the client."
    "You paint the painting by saying [PAINT: short features separated by "
    "commas] and you can show the painting by saying [SHOW]."
    "Note that generating a painting may take some time. You start the "
    "conversation by saying \"Hi\" to your client. Keep it short!";

// short painting description
// -> short painting features separated by commas

int main() {
  printf("Loading LLM...\n");
  // std::cout << "Enter Prompt: ";
  // std::getline(std::cin, params.prompt);

  // total length of the sequence including the prompt
  const int n_len = 32;

  // init LLM

  llama_backend_init();
  llama_numa_init(GGML_NUMA_STRATEGY_DISABLED);

  // initialize the model

  llama_model_params model_params = llama_model_default_params();

  // model_params.n_gpu_layers = 99; // offload all layers to the GPU

  llama_model *model = llama_load_model_from_file(path, model_params);

  if (model == NULL) {
    fprintf(stderr, "%s: error: unable to load model\n", __func__);
    return 1;
  }

  // initialize the context

  llama_context_params ctx_params = llama_context_default_params();

  ctx_params.seed = -1;
  ctx_params.n_ctx = 2048;
  ctx_params.n_threads = ctx_params.n_threads_batch = get_num_physical_cores();

  llama_context *ctx = llama_new_context_with_model(model, ctx_params);

  if (ctx == NULL) {
    fprintf(stderr, "%s: error: failed to create the llama_context\n",
            __func__);
    return 1;
  }

  // tokenize the prompt

  std::vector<llama_token> tokens_list = ::llama_tokenize(
      ctx, "<|system|>\n" + std::string(prompt) + "</s>\n" + "<|ai painter|>\n",
      true);

  const int n_ctx = llama_n_ctx(ctx);
  const int n_kv_req = tokens_list.size() + (n_len - tokens_list.size());

  // LOG_TEE("\n%s: n_len = %d, n_ctx = %d, n_kv_req = %d\n", __func__, n_len,
  // n_ctx, n_kv_req);

  // make sure the KV cache is big enough to hold all the prompt and generated
  // tokens
  if (n_kv_req > n_ctx) {
    LOG_TEE("%s: error: n_kv_req > n_ctx, the required KV cache size is not "
            "big enough\n",
            __func__);
    LOG_TEE("%s:        either reduce n_len or increase n_ctx\n", __func__);
    return 1;
  }

  // create a llama_batch with size 512
  // we use this object to submit token data for decoding

  llama_batch batch = llama_batch_init(512, 0, 1);

  // evaluate the initial prompt
  for (size_t i = 0; i < tokens_list.size(); i++) {
    llama_batch_add(batch, tokens_list[i], i, {0}, false);
  }

  // llama_decode will output logits only for the last token of the prompt
  batch.logits[batch.n_tokens - 1] = true;

  if (llama_decode(ctx, batch) != 0) {
    LOG_TEE("%s: llama_decode() failed\n", __func__);
    return 1;
  }
  batch.logits[batch.n_tokens - 1] = false;

  // llama_batch_clear(batch);

  // main loop
  printf("\n<|system|>\n%s\n<|ai painter|>\n", prompt);

  int n_cur = batch.n_tokens;

  while (true) {
    // Get User Input
    {
      // Read User Input
      char user_in[128];
      memset(user_in, 0, sizeof(user_in));

      printf("<|user|> (n_cur=%i, batch.n_tokens=%i)\n", n_cur, batch.n_tokens);
      fgets(user_in, sizeof(user_in), stdin);

      if (strlen(user_in) == 0) {
        strcpy(user_in, "Would you paint a portrait of me?");
        printf("[AUTO-PROMPT]: %s\n", user_in);
      }

      printf("<|ai painter|>\n");

      // Tokenize User Input
      std::vector<llama_token> user_tokens = ::llama_tokenize(
          ctx,
          "<|user|>\n" + std::string(user_in) + "</s>\n" + "<|ai painter|>\n",
          true);

      // evaluate the initial prompt
      for (size_t i = 0; i < user_tokens.size(); i++) {
        llama_batch_add(batch, user_tokens[i], n_cur + i, {0}, false);
      }

      n_cur += user_tokens.size();

      // batch.logits[batch.n_tokens - 1] = true;

      batch.logits[batch.n_tokens - 1] = true;

      if (llama_decode(ctx, batch) != 0) {
        LOG_TEE("%s: llama_decode() failed\n", __func__);
        return 1;
      }
      batch.logits[batch.n_tokens - 1] = false;
    }

    while (true) {
      // sample the next token
      {
        auto n_vocab = llama_n_vocab(model);
        auto *logits = llama_get_logits_ith(ctx, batch.n_tokens - 1);

        std::vector<llama_token_data> candidates;
        candidates.reserve(n_vocab);

        for (llama_token token_id = 0; token_id < n_vocab; token_id++) {
          candidates.emplace_back(
              llama_token_data{token_id, logits[token_id], 0.0f});
        }

        llama_token_data_array candidates_p = {candidates.data(),
                                               candidates.size(), false};

        // sample the most likely token
        const llama_token new_token_id =
            llama_sample_token_greedy(ctx, &candidates_p);

        // is it an end of stream?
        if (new_token_id == llama_token_eos(model)) // || n_cur == n_len)
        {
          LOG_TEE("\n");

          break;
        }

        LOG_TEE("%s", llama_token_to_piece(ctx, new_token_id).c_str());
        fflush(stdout);

        // prepare the next batch
        llama_batch_clear(batch);

        // push this new token for next evaluation
        llama_batch_add(batch, new_token_id, n_cur, {0}, true);
      }

      n_cur += 1;

      // evaluate the current batch with the transformer model
      if (llama_decode(ctx, batch)) {
        fprintf(stderr, "%s : failed to eval, return code %d\n", __func__, 1);
        return 1;
      }
    }

    llama_batch_clear(batch);

    // asd
  }

  LOG_TEE("\n");

  fprintf(stderr, "\n");

  llama_batch_free(batch);

  llama_free(ctx);
  llama_free_model(model);

  llama_backend_free();

  return 0;
}