#pragma once

#include <llama.h>
#include <string>

// Wrapper Class for the llama.cpp library
class LLM {
public:
  LLM(const char *model, const char *prompt);
  ~LLM();

    // Use in loop for chat-like behaviour
    std::string reply(std::string prompt);

private:
  struct {
    llama_model *model{};
    llama_context *ctx{};
    llama_batch batch{};
  } llama;
};