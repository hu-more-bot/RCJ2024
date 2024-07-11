#pragma once

#include "common-sdl.h"
#include "grammar-parser.h"

#include "common.h"
#include <string>
#include <vector>

// command-line parameters
struct whisper_params
{
  int32_t n_threads = std::min(4, (int32_t)std::thread::hardware_concurrency());
  int32_t prompt_ms = 5000;
  int32_t command_ms = 8000;
  int32_t capture_id = -1;
  int32_t max_tokens = 32;
  int32_t audio_ctx = 0;

  float vad_thold = 0.6f;
  float freq_thold = 100.0f;

  float grammar_penalty = 100.0f;

  grammar_parser::parse_state grammar_parsed;

  bool speed_up = false;
  bool translate = false;
  bool print_special = false;
  bool print_energy = false;
  bool no_timestamps = true;
  bool use_gpu = true;
  bool flash_attn = false;

  std::string language = "en";
  std::string model = "models/ggml-base.en.bin";
  std::string fname_out;
  std::string commands;
  std::string prompt;
  std::string context;
  std::string grammar;

  // A regular expression that matches tokens to suppress
  std::string suppress_regex;
};

// Wrapper Class for the whisper.cpp library
class STT
{
public:
  STT(const char *model);
  ~STT();

  std::string listen();

private:
  audio_async audio;
  whisper_params params;
  struct whisper_context *ctx{};
};