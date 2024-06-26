#include <tts.h>

#include <Artifex/log.h>

#include <espeak-ng/speak_lib.h>
#include <onnxruntime_c_api.h>

#include <stdio.h>
#include <stdlib.h>

#include <jsmn.h>

#define TAG "tts"

// Simple TTS Instance
struct TTS {
  const OrtApi *ort;
  OrtEnv *env;
  OrtSessionOptions *options;
  OrtSession *session;

  // PiperConfig
  std::string eSpeakDataPath;
  bool useESpeak = true;

  bool useTashkeel = false;
  std::optional<std::string> tashkeelModelPath;
  std::unique_ptr<tashkeel::State> tashkeelState;

  // PiperVoice
  json configRoot;
  PhonemizeConfig phonemizeConfig;
  SynthesisConfig synthesisConfig;
  ModelConfig modelConfig;
  ModelSession session;
};

int ttsIsOK(tts_t tts) {
  // TODO
}

int ttsCreate(tts_t *tts, const char *path) {
  if (!tts || !path)
    return 1;

  *tts = malloc(sizeof(struct TTS));
  if (!tts) {
    ax_error(TAG, "failed to allocate memory");
    return 1;
  }

  jsmn_parser p;
  jsmntok_t t[1024];
  int r;

  // struct of which properties the config file contains (with ids)
  struct {
    int espeak, phoneme_type, phoneme_id_map, phoneme_map;
    int audio, inference, length_scale, noise_w, phoneme_silence;
    int speaker_id_map;
  } contains = {};

  /* Load Config File */ {
    // Load File
    FILE *f = fopen(path, "r");
    if (!f) {
      free(*tts);
      *tts = NULL;

      ax_error(TAG, "failed to open config file");
      return 1;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char buf[size];
    if (fread(buf, 1, size, f) != size) {
      ax_warning(TAG, "reading config file partially failed");
    }

    // Parse JSON
    jsmn_init(&p);
    if ((r = jsmn_parse(&p, buf, size, t, sizeof(t) / sizeof(t[0]))) < 0) {
      free(*tts);
      *tts = NULL;

      ax_error(TAG, "failed to parse config file");
      // may have run out of tokens

      return 1;
    }

    // Query Tokens
    // for (int i = 1; i < r; i++) {
    //   if (t[i].type)
    // }
  }

  /* Parse Phonemize Config */ {
    // TODO
  }

  /* Parse Synthesis Config */ {
    // TODO
  }

  /* Parse Model Config */ {
    // TODO
  }

  /* Load Model */ {
    // TODO
  }

  /* Initialize */ {
    // Set up espeak-ng for calling espeak_TextToPhonemesWithTerminator
    // See: https://github.com/rhasspy/espeak-ng
    // spdlog::debug("Initializing eSpeak");
    int result = espeak_Initialize(AUDIO_OUTPUT_SYNCHRONOUS,
                                   /*buflength*/ 0,
                                   /*path*/ config.eSpeakDataPath.c_str(),
                                   /*options*/ 0);
    if (result < 0) {
      throw std::runtime_error("Failed to initialize eSpeak-ng");
    }
  }

  ax_verbose(TAG, "initialized");
  return 0;
}

// Destroy tts
void ttsDestroy(tts_t *tts);