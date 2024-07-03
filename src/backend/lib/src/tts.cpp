#include <tts.h>

#include <Artifex/log.h>

#define TAG "tts"

#include <piper.hpp>

#include <optional>
#include <string>

struct TTS {
  piper::PiperConfig config;
  piper::Voice voice;
};

void tts__destroy(tts_t tts) { piper::terminate(tts->config); }

// std::vector<int16_t> TTS::say(std::string text) {
//   std::vector<int16_t> out;

//   piper::SynthesisResult result;
//   piper::textToAudio(tts.config, tts.voice, text, out, result, nullptr);

//   return out;
// }

// bool TTS::sayToFile(std::string text, const char *output) {
//   // Output audio to WAV file
//   std::ofstream audioFile(output, std::ios::binary);

//   if (!audioFile.is_open())
//     return true;

//   piper::SynthesisResult result;
//   piper::textToWavFile(tts.config, tts.voice, text, audioFile, result);

//   audioFile.close();

//   return false;
// }

int ttsIsOK(tts_t tts) {
  if (!tts)
    return 0;
  return 1;
}

#include <iostream>

int ttsCreate(tts_t *tts, const char *path) {
  if (!tts)
    return 1;

  tts_t t = *tts = (tts_t)malloc(sizeof(struct TTS));
  if (!t) {
    ax_error(TAG, "failed to allocate memory");
    return 1;
  }

  std::string cpppath = std::string(path) + "/espeak-ng-data";
  t->config.eSpeakDataPath = &cpppath;
  printf("asd\n");
  std::string jsonpath = std::string(path) +
                         ".onnx.json" std::optional<piper::SpeakerId> speakerId;
  piper::loadVoice(t->config, std::string(path) + ".onnx", , t->voice,
                   speakerId, false);

  piper::initialize(t->config);

  ax_verbose(TAG, "initialized");
  return 0;
}

void ttsDestroy(tts_t *tts) {
  if (!tts || !*tts)
    return;

  tts__destroy(*tts);

  free(*tts);
  *tts = NULL;

  ax_verbose(TAG, "destroyed");
}