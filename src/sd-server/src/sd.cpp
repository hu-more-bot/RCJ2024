#include <sd.hpp>

#include <stdexcept>

SD::SD(const char *model) {
  int n_threads = -1;
  sd_type_t wtype = SD_TYPE_COUNT;
  rng_type_t rng_type = CUDA_RNG;
  schedule_t schedule = DEFAULT;
  bool control_net_cpu = false;

  ctx = new_sd_ctx(model, "", "", "", "", "", false, false, true, n_threads,
                   wtype, rng_type, schedule, control_net_cpu);

  if (ctx == NULL)
    throw std::runtime_error(std::string(__func__) + ": new_sd_ctx_t failed");
}

SD::~SD() {
  free(result->data);
  free(result);

  free_sd_ctx(ctx);
}

bool SD::generate() {
  free(result->data);
  free(result);

  result = txt2img(ctx, config.prompt, config.negative_prompt, config.clip_skip,
                   config.cfg_scale, config.width, config.height,
                   config.sample_method, config.sample_steps, config.seed,
                   config.batch_count,
                   NULL, // control image
                   config.control_strength);

  if (result)
    return true;
  return false;
}