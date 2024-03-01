#pragma once

#include <llama.h>

// Wrapper Class for the llama.cpp library
class LLM {
public:
  LLM(const char *model, const char *prompt);
  ~LLM();

private:
  struct {
    llama_model *model{};
    llama_context *ctx{};
    llama_batch batch{};
  } llama;
};