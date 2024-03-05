#pragma once

#include <llama.h>

#include <string>
#include <vector>

// Wrapper Class for the llama.cpp library
class LLM {
public:
  LLM(const char *model, const char *prompt);
  ~LLM();

  // Use in loop for chat-like behaviour
  std::string reply(std::string prompt);

private:
  std::vector<llama_token> tokenize(std::string text);
  bool decode(std::vector<llama_token> tokens);

  bool decode(std::string text);

  struct {
    llama_model *model{};
    llama_context *ctx{};
    llama_batch batch{};

    int n_cur{};
  } llm;
};