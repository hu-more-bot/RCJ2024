#include <sd.hpp>

#include <unistd.h>

#include <Artifex/log.h>

#define TAG "sd"

SD::SD(const char *model) {
  ctx = new_sd_ctx(model, "", "", "", "", "", false, false, true, -1,
                   SD_TYPE_COUNT, STD_DEFAULT_RNG, DEFAULT, false);

  if (ctx == NULL) {
    ax_error(TAG, "failed to create context");
    return;
  }

  ax_verbose(TAG, "initialized");
}

SD::~SD() {
  free(result->data);
  free(result);

  free_sd_ctx(ctx);

  ax_verbose(TAG, "destroyed");
}

bool SD::generate() {
  if (result) {
    free(result->data);
    free(result);
    result = NULL;
  }

  result = txt2img(ctx, config.prompt, config.negative_prompt, config.clip_skip,
                   config.cfg_scale, config.width, config.height,
                   config.sample_method, config.sample_steps, config.seed,
                   config.batch_count,
                   NULL, // control image
                   config.control_strength);

  if (result) {
    ax_debug(TAG, "generted image");
    return true;
  }

  ax_warning(TAG, "failed to generate image");
  return false;
}