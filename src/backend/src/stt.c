#include <stt.h>

#include <aaudio.h>
#include <whisper.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <Artifex/clock.h>
#include <Artifex/log.h>

#define TAG "stt"

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

struct STT {
  struct whisper_context *ctx;
  aaudio_t audio;
};

int sttIsOK(stt_t stt) {
  if (!stt || !stt->ctx)
    return 0;
  return 1;
}

void stt_cb_log_disable(enum ggml_log_level _, const char *__, void *___) {}

int sttCreate(stt_t *stt, const char *path) {
  if (!stt)
    return 1;

  stt_t s = *stt = malloc(sizeof(struct STT));
  if (!s) {
    ax_error(TAG, "failed to allocate memory");
    return 1;
  }

  // Disable Logging
  whisper_log_set(stt_cb_log_disable, NULL);

  // whisper init
  struct whisper_context_params cparams = whisper_context_default_params();

  if (!(s->ctx = whisper_init_from_file_with_params(path, cparams))) {
    free(s);
    s = NULL;

    ax_error(TAG, "failed to create context");
    return 1;
  }

  // open audio device
  if (aauidoCreate(&s->audio, WHISPER_SAMPLE_RATE, 1024)) {
    whisper_free(s->ctx);

    free(s);
    s = NULL;

    ax_error(TAG, "failed to init audio");
    return 1;
  }

  ax_verbose(TAG, "initialized");
  return 0;
}

void sttDestroy(stt_t *stt) {
  if (!stt || !*stt)
    return;

  aaudioDestroy(&(*stt)->audio);
  whisper_free((*stt)->ctx);

  free(*stt);
  *stt = NULL;

  ax_verbose(TAG, "destroyed");
}

// grammar_parser::parse_state grammar_parsed;

const char *transcribe(struct whisper_context *ctx, const float *pcmf32,
                       unsigned long len, const char *grammar_rule,
                       float *logprob_min, float *logprob_sum, int *n_tokens);

int vad_simple(float *pcmf32, unsigned long len, int sample_rate, int last_ms,
               float vad_thold, float freq_thold, int verbose);

void sleep(float time);

void high_pass_filter(float *data, unsigned long len, float cutoff,
                      float sample_rate);

int sttListen(stt_t stt, char *text, unsigned long len) {
  if (!sttIsOK(stt))
    return 1;

  aaudioStart(stt->audio, 8192);

  // wait for 1 second to avoid any buffered noise
  sleep(1.0f);

  aaudioClear(stt->audio);

  int is_running = true;
  int have_prompt = false;

  float logprob_min0 = 0.0f;
  float logprob_min = 0.0f;

  float logprob_sum0 = 0.0f;
  float logprob_sum = 0.0f;

  int n_tokens0 = 0;
  int n_tokens = 0;

  // main loop
  while (true) {
    sleep(0.1);

    // alcCaptureSamples(stt->device, ALCvoid * buffer, ALCsizei samples)
    // audio.get(2000, pcmf32_cur);
    float buffer[2000];
    unsigned long len = aaudioGet(stt->audio, buffer, 2000);

    float vad_thold = 0.6f;
    float freq_thold = 100.0f;

    if (vad_simple(buffer, len, WHISPER_SAMPLE_RATE, 1000, vad_thold,
                   freq_thold, false)) {
      ax_debug(TAG, "speech detected");
      {
        unsigned long silence = 3.0f * WHISPER_SAMPLE_RATE;
        float buffer[silence + 8000];
        memset(buffer, 0, silence + 8000);
        unsigned long len = aaudioGet(stt->audio, buffer + silence, 8000);
        // audio.get(8000, pcmf32_cur); // 8000 = command_ms

        const char *txt =
            // TODO remove first white space from before and after text
            // trim(
            transcribe(stt->ctx, buffer, len, "root", &logprob_min,
                       &logprob_sum, &n_tokens);
        //);

        // const float p = 100.0f * std::exp((logprob - logprob0) / (n_tokens
        // - n_tokens0));
        const float p = 100.0f * exp(logprob_min);

        aaudioStop(stt->audio);

        return txt;
      }

      aaudioClear(stt->audio);
    }
  }

  return 0;
}

// impl

int vad_simple(float *pcmf32, unsigned long len, int sample_rate, int last_ms,
               float vad_thold, float freq_thold, int verbose) {
  const int n_samples = len;
  const int n_samples_last = (sample_rate * last_ms) / 1000;

  if (n_samples_last >= n_samples) {
    // not enough samples - assume no speech
    return false;
  }

  if (freq_thold > 0.0f) {
    high_pass_filter(pcmf32, len, freq_thold, sample_rate);
  }

  float energy_all = 0.0f;
  float energy_last = 0.0f;

  for (int i = 0; i < n_samples; i++) {
    energy_all += fabsf(pcmf32[i]);

    if (i >= n_samples - n_samples_last) {
      energy_last += fabsf(pcmf32[i]);
    }
  }

  energy_all /= n_samples;
  energy_last /= n_samples_last;

  if (energy_last > vad_thold * energy_all) {
    return false;
  }

  return true;
}

void sleep(float time) {
  float start = axClockNow();
  while (axClockNow() < start + time)
    ;
}

// grammar_parser::parse_state grammar_parsed;

const char *transcribe(struct whisper_context *ctx, const float *pcmf32,
                       unsigned long len, const char *grammar_rule,
                       float *logprob_min, float *logprob_sum, int *n_tokens) {
  *logprob_min = 0.0f;
  *logprob_sum = 0.0f;
  *n_tokens = 0;

  // whisper_full_params wparams =
  // whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
  struct whisper_full_params wparams =
      whisper_full_default_params(WHISPER_SAMPLING_BEAM_SEARCH);

  wparams.print_progress = false;
  wparams.print_special = false;
  wparams.print_realtime = false;
  wparams.print_timestamps = false;
  wparams.translate = false;
  wparams.no_context = true;
  wparams.no_timestamps = true;
  wparams.single_segment = true;
  wparams.max_tokens = 32;
  wparams.language = "en";
  wparams.n_threads = 4;

  wparams.audio_ctx = 0;
  wparams.speed_up = false;

  wparams.temperature = 0.4f;
  wparams.temperature_inc = 1.0f;
  wparams.greedy.best_of = 5;

  wparams.beam_search.beam_size = 5;

  wparams.initial_prompt = params.context.data();

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

  if (whisper_full(ctx, wparams, pcmf32, len) != 0) {
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
      logprob_min = MIN(logprob_min, token.plog);
      logprob_sum += token.plog;
      ++n_tokens;
    }
  }

  return result;
}

void high_pass_filter(float *data, unsigned long len, float cutoff,
                      float sample_rate) {
  const float rc = 1.0f / (2.0f * M_PI * cutoff);
  const float dt = 1.0f / sample_rate;
  const float alpha = dt / (rc + dt);

  float y = data[0];

  for (size_t i = 1; i < len; i++) {
    y = alpha * (y + data[i] - data[i - 1]);
    data[i] = y;
  }
}

// remove first white space from before and after text
// std::string trim(const std::string &s) {
//   std::regex e("^\\s+|\\s+$");
//   return std::regex_replace(s, e, "");
// }