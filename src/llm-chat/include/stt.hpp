#pragma once

#include "whisper/whisper.h"

#include <string>
#include <vector>

// Wrapper Class for the whisper.cpp library
class STT {
public:
  STT(const char *model);
  ~STT();

  void setLang(std::string lang = "auto", bool transl = false);

  std::string listenBuf(std::vector<float> pcmf32);
  std::string listenWav(const char *path);

private:
  std::string language = "auto";
  bool translate = false;

  struct whisper_context *ctx{};
};