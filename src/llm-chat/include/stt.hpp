#pragma once

#include "common-sdl.h"
#include "whisper/whisper.h"

#include <string>
#include <vector>

// Wrapper Class for the whisper.cpp library
class STT {
public:
  STT(const char *model);
  ~STT();

  void setLang(std::string lang = "auto", bool transl = false);

  std::string listen();

private:
  std::string language = "auto";
  bool translate = false;

  audio_async audio;

  struct whisper_context *ctx{};
};