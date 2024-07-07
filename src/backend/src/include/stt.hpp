#pragma once

#include "whisper/common-sdl.h"
#include "whisper/whisper.h"

#include <string>
#include <vector>

// Wrapper Class for the whisper.cpp library
class STT {
public:
  STT(const char *model);
  ~STT();

  std::string listen();

private:
  audio_async audio;

  struct whisper_context *ctx{};
};