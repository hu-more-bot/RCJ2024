#pragma once

#include "piper.hpp"

class TTS
{
public:
  TTS(const char *model,
      const char *eSpeakData = "../models/piper/espeak-ng-data");
  ~TTS();

  std::vector<int16_t> say(std::string text);

private:
  struct
  {
    piper::PiperConfig config;
    piper::Voice voice;
  } tts;
};