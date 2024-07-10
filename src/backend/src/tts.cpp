#include <tts.hpp>

#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <Artifex/log.h>

#define TAG "tts"

TTS::TTS(const char *model, const char *eSpeakData)
{
  // tts.config.eSpeakDataPath = std::string(argv[2]);
  // auto outputPath = std::string(argv[3]);

  tts.config.eSpeakDataPath = eSpeakData;

  std::optional<piper::SpeakerId> speakerId;
  loadVoice(tts.config, model, std::string(model) + ".json", tts.voice,
            speakerId, false);

  piper::initialize(tts.config);

  ax_verbose(TAG, "initialized");
}

TTS::~TTS()
{
  piper::terminate(tts.config);
  ax_verbose(TAG, "destroyed");
}

std::vector<int16_t> TTS::say(std::string text)
{
  std::vector<int16_t> out;

  piper::SynthesisResult result;
  piper::textToAudio(tts.config, tts.voice, text, out, result, nullptr);

  return out;
}