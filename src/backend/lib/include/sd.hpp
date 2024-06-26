#pragma once

#include <stable-diffusion.h>

#include <string>

class SD
{
public:
  SD(const char *model);
  ~SD();

  // img2img
  bool generate();

  bool show = false;
  sd_image_t *result = NULL;

  struct
  {
    char prompt[128] = "";
    char negative_prompt[128] = "";

    float min_cfg = 1.0f;
    float cfg_scale = 7.0f;
    int clip_skip = -1; // <= 0 represents unspecified

    int width = 512;
    int height = 512;

    sample_method_t sample_method = EULER_A;
    int sample_steps = 20;
    int64_t seed = 42;

    int batch_count = 1;

    float control_strength = 0.9f;
  } config;

private:
  sd_ctx_t *ctx{};
};