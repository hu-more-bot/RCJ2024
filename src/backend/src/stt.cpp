#include <Artifex/log.h>

#include <stt.hpp>

#include "whisper.h"

#include <unistd.h>

#define TAG "stt"

std::string transcribe(whisper_context *ctx, const whisper_params &params,
                       const std::vector<float> &pcmf32,
                       const std::string &grammar_rule, float &logprob_min,
                       float &logprob_sum, int &n_tokens, int64_t &t_ms);
void stt_cb_log_disable(enum ggml_log_level, const char *, void *) {}

STT::STT(const char *model) : audio(30 * 1000) {
  // Disable Logging
  whisper_log_set(stt_cb_log_disable, NULL);

  // whisper init

  struct whisper_context_params cparams = whisper_context_default_params();

  cparams.use_gpu = true;
  cparams.flash_attn = false;

  if (!(ctx = whisper_init_from_file_with_params(model, cparams))) {
    ax_error(TAG, "failed to init");
    return;
  }

  // init audio

  if (!audio.init(-1, WHISPER_SAMPLE_RATE)) {
    ax_error(TAG, "failed to init audio");
    return;
  }

  ax_verbose(TAG, "initialized");
}

STT::~STT() {
  audio.~audio_async();
  whisper_free(ctx);
  ax_verbose(TAG, "destroyed");
}

std::string STT::listen() {
  audio.resume();

  ax_debug(TAG, "listening");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  audio.clear();

  int ret_val = 0;

  if (!params.grammar.empty()) {
    auto &grammar = params.grammar_parsed;
    if (is_file_exist(params.grammar.c_str())) {
      // read grammar from file
      std::ifstream ifs(params.grammar.c_str());
      const std::string txt = std::string((std::istreambuf_iterator<char>(ifs)),
                                          std::istreambuf_iterator<char>());
      grammar = grammar_parser::parse(txt.c_str());
    } else {
      // read grammar from string
      grammar = grammar_parser::parse(params.grammar.c_str());
    }

    // will be empty (default) if there are parse errors
    if (grammar.rules.empty()) {
      ret_val = 1;
    } else {
      fprintf(stderr, "%s: grammar:\n", __func__);
      grammar_parser::print_grammar(stderr, grammar);
      fprintf(stderr, "\n");
    }
  }

  float logprob_min0 = 0.0f;
  float logprob_min = 0.0f;

  float logprob_sum0 = 0.0f;
  float logprob_sum = 0.0f;

  int n_tokens0 = 0;
  int n_tokens = 0;

  std::vector<float> pcmf32_cur;
  std::vector<float> pcmf32_prompt;

  std::string out;

  // main loop
  while (true) {
    sdl_poll_events();
    // delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    audio.get(2000, pcmf32_cur);

    if (::vad_simple(pcmf32_cur, WHISPER_SAMPLE_RATE, 1000, params.vad_thold,
                     params.freq_thold, params.print_energy)) {
      fprintf(stdout, "%s: Speech detected! Processing ...\n", __func__);

      int64_t t_ms = 0;

      // we have heard the activation phrase, now detect the commands
      audio.get(params.command_ms, pcmf32_cur);

      // prepend 3 second of silence
      pcmf32_cur.insert(pcmf32_cur.begin(), 3.0f * WHISPER_SAMPLE_RATE, 0.0f);

      // prepend the prompt audio
      pcmf32_cur.insert(pcmf32_cur.begin(), pcmf32_prompt.begin(),
                        pcmf32_prompt.end());

      out = ::trim(::transcribe(ctx, params, pcmf32_cur, "root", logprob_min,
                                logprob_sum, n_tokens, t_ms));

      audio.clear();
      break;
    }
  }

  audio.pause();

  return out;
}

std::string transcribe(whisper_context *ctx, const whisper_params &params,
                       const std::vector<float> &pcmf32,
                       const std::string &grammar_rule, float &logprob_min,
                       float &logprob_sum, int &n_tokens, int64_t &t_ms) {
  const auto t_start = std::chrono::high_resolution_clock::now();

  logprob_min = 0.0f;
  logprob_sum = 0.0f;
  n_tokens = 0;
  t_ms = 0;

  // whisper_full_params wparams =
  // whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
  whisper_full_params wparams =
      whisper_full_default_params(WHISPER_SAMPLING_BEAM_SEARCH);

  wparams.print_progress = false;
  wparams.print_special = params.print_special;
  wparams.print_realtime = false;
  wparams.print_timestamps = !params.no_timestamps;
  wparams.translate = params.translate;
  wparams.no_context = true;
  wparams.no_timestamps = params.no_timestamps;
  wparams.single_segment = true;
  wparams.max_tokens = params.max_tokens;
  wparams.language = params.language.c_str();
  wparams.n_threads = params.n_threads;

  wparams.audio_ctx = params.audio_ctx;
  wparams.speed_up = params.speed_up;

  wparams.temperature = 0.4f;
  wparams.temperature_inc = 1.0f;
  wparams.greedy.best_of = 5;

  wparams.beam_search.beam_size = 5;

  wparams.initial_prompt = params.context.data();

  wparams.suppress_regex = params.suppress_regex.c_str();

  const auto &grammar_parsed = params.grammar_parsed;
  auto grammar_rules = grammar_parsed.c_rules();

  if (!params.grammar_parsed.rules.empty() && !grammar_rule.empty()) {
    if (grammar_parsed.symbol_ids.find(grammar_rule) ==
        grammar_parsed.symbol_ids.end()) {
      fprintf(stderr,
              "%s: warning: grammar rule '%s' not found - skipping grammar "
              "sampling\n",
              __func__, grammar_rule.c_str());
    } else {
      wparams.grammar_rules = grammar_rules.data();
      wparams.n_grammar_rules = grammar_rules.size();
      wparams.i_start_rule = grammar_parsed.symbol_ids.at(grammar_rule);
      wparams.grammar_penalty = params.grammar_penalty;
    }
  }

  if (whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size()) != 0) {
    return "";
  }

  std::string result;

  const int n_segments = whisper_full_n_segments(ctx);
  for (int i = 0; i < n_segments; ++i) {
    const char *text = whisper_full_get_segment_text(ctx, i);

    result += text;

    const int n = whisper_full_n_tokens(ctx, i);
    for (int j = 0; j < n; ++j) {
      const auto token = whisper_full_get_token_data(ctx, i, j);

      if (token.plog > 0.0f)
        exit(0);
      logprob_min = std::min(logprob_min, token.plog);
      logprob_sum += token.plog;
      ++n_tokens;
    }
  }

  const auto t_end = std::chrono::high_resolution_clock::now();
  t_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start)
             .count();

  return result;
}