#pragma once

#include "piper.hpp"

class TTS
{
public:
  TTS(std::string model, std::string voice);
  ~TTS();

  // Outputs 44100 MONO
  std::vector<int16_t> say(std::string text);
  bool sayToFile(std::string text, const char *output);

private:
  struct
  {
    piper::PiperConfig config;
    piper::Voice voice;
  } tts;
};