// Voice assistant example
//
// Speak short text commands to the microphone.
// This program will detect your voice command and convert them to text.
//
// ref: https://github.com/ggerganov/whisper.cpp/issues/171
//

#include <cassert>
#include <cstdio>
#include <fstream>
#include <map>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

// command-line parameters
struct whisper_params {
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

int main(int argc, char **argv) {

  whisper_print_timings(ctx);
  whisper_free(ctx);

  return ret_val;
}
