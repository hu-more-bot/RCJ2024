#pragma once

#include "whisper.h"
#include "common-sdl.h"

#include <string>
#include <vector>

// Wrapper Class for the whisper.cpp library
class STT
{
public:
  STT(const char *model);
  ~STT();

  std::string listen();

private:
  audio_async audio;
  struct whisper_context *ctx{};
};