#include <stt.hpp>

#include <stdexcept>

void cb_log_disable(enum ggml_log_level, const char *, void *) {}

STT::STT(const char *model) {
  // Disable Logging
  whisper_log_set(cb_log_disable, NULL);

  // Initialize Context
  struct whisper_context_params cparams = whisper_context_default_params();
  cparams.use_gpu = false; // TODO

  if (!(ctx = whisper_init_from_file_with_params(model, cparams)))
    throw std::runtime_error(std::string(__func__) +
                             ": failed to initialize context");

  const char *openvino_encode_device = "CPU";

  // Initialize OpenVino encoder
  whisper_ctx_init_openvino_encoder(ctx, NULL, openvino_encode_device, NULL);
}

STT::~STT() { whisper_free(ctx); }

void STT::setLang(std::string lang, bool transl) {
  language = lang, translate = transl;

  // Set Language
  if (!whisper_is_multilingual(ctx)) {
    printf("%s: model is not multilingual", __func__);

    if (translate) {
      fprintf(stderr, ", ignoring translation");
    }

    if (language != "en" && language != "auto") {
      fprintf(stderr, "%s language", translate ? " and " : ", ignoring ");
      language = "auto";
    }

    translate = false;

    fprintf(stderr, "\n");
  }
}

std::string STT::listenBuf(std::vector<float> pcmf32) {
  // run the inference
  {
    whisper_full_params wparams =
        whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

    wparams.strategy = params.beam_size > 1 ? WHISPER_SAMPLING_BEAM_SEARCH
                                            : WHISPER_SAMPLING_GREEDY;

    wparams.print_realtime = false;
    wparams.print_progress = params.print_progress;
    wparams.print_timestamps = !params.no_timestamps;
    wparams.print_special = params.print_special;
    wparams.translate = translate;
    wparams.language = language.c_str();
    wparams.detect_language = language == "auto";
    wparams.n_threads = params.n_threads;
    wparams.n_max_text_ctx =
        params.max_context >= 0 ? params.max_context : wparams.n_max_text_ctx;
    wparams.offset_ms = params.offset_t_ms;
    wparams.duration_ms = params.duration_ms;

    wparams.token_timestamps =
        params.output_wts || params.output_jsn_full || params.max_len > 0;
    wparams.thold_pt = params.word_thold;
    wparams.max_len =
        params.output_wts && params.max_len == 0 ? 60 : params.max_len;
    wparams.split_on_word = params.split_on_word;
    wparams.audio_ctx = params.audio_ctx;

    wparams.speed_up = params.speed_up;
    wparams.debug_mode = params.debug_mode;

    wparams.tdrz_enable = params.tinydiarize; // [TDRZ]

    wparams.initial_prompt = params.prompt.c_str();

    wparams.greedy.best_of = params.best_of;
    wparams.beam_search.beam_size = params.beam_size;

    wparams.temperature_inc =
        params.no_fallback ? 0.0f : wparams.temperature_inc;
    wparams.entropy_thold = params.entropy_thold;
    wparams.logprob_thold = params.logprob_thold;

    wparams.no_timestamps = params.no_timestamps;

    whisper_print_user_data user_data = {&params, &pcmf32s, 0};

    // this callback is called on each new segment
    if (!wparams.print_realtime) {
      wparams.new_segment_callback = whisper_print_segment_callback;
      wparams.new_segment_callback_user_data = &user_data;
    }

    if (wparams.print_progress) {
      wparams.progress_callback = whisper_print_progress_callback;
      wparams.progress_callback_user_data = &user_data;
    }

    // examples for abort mechanism
    // in examples below, we do not abort the processing, but we could if the
    // flag is set to true

    // the callback is called before every encoder run - if it returns false,
    // the processing is aborted
    {
      static bool is_aborted =
          false; // NOTE: this should be atomic to avoid data race

      wparams.encoder_begin_callback = [](struct whisper_context * /*ctx*/,
                                          struct whisper_state * /*state*/,
                                          void *user_data) {
        bool is_aborted = *(bool *)user_data;
        return !is_aborted;
      };
      wparams.encoder_begin_callback_user_data = &is_aborted;
    }

    // the callback is called before every computation - if it returns true, the
    // computation is aborted
    {
      static bool is_aborted =
          false; // NOTE: this should be atomic to avoid data race

      wparams.abort_callback = [](void *user_data) {
        bool is_aborted = *(bool *)user_data;
        return is_aborted;
      };
      wparams.abort_callback_user_data = &is_aborted;
    }

    if (whisper_full_parallel(ctx, wparams, pcmf32.data(), pcmf32.size(),
                              params.n_processors) != 0) {
      fprintf(stderr, "%s: failed to process audio\n", argv[0]);
      return 10;
    }
  }

  // output stuff
  {
    printf("\n");

    // output to text file
    if (params.output_txt) {
      const auto fname_txt = fname_out + ".txt";
      output_txt(ctx, fname_txt.c_str(), params, pcmf32s);
    }
  }
}

std::string STT::listenWav(const char *path, std::string language) {
  const auto fname_inp = params.fname_inp[f];
  const auto fname_out =
      f < (int)params.fname_out.size() && !params.fname_out[f].empty()
          ? params.fname_out[f]
          : params.fname_inp[f];

  std::vector<float> pcmf32; // mono-channel F32 PCM
  // std::vector<std::vector<float>> pcmf32s; // stereo-channel F32 PCM

  if (!::read_wav(fname_inp, pcmf32, pcmf32s, params.diarize)) {
    fprintf(stderr, "error: failed to read WAV file '%s'\n", fname_inp.c_str());
    continue;
  }

  return listenBuf(pcmf32, language);
}