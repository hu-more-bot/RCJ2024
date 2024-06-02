#pragma once

#include <functional>
#include <llama.h>

#include <string>
#include <vector>

// AI Chat wrapper for the llama.cpp library
class LLM
{
public:
  LLM(const char *model, const char *prompt);
  ~LLM();

  bool decode(std::string author, std::string text);
  std::string generate(std::function<void(std::string)> onNewToken = {});

  // Use in loop for chat-like behaviour
  std::string reply(std::string author, std::string prompt,
                    std::function<void(std::string)> onNewToken = {});

  // Save KV Cache state
  bool save(const char *path);

  // Load KV Cache state
  bool load(const char *path);

private:
  std::vector<llama_token> tokenize(std::string text);
  bool decode(std::vector<llama_token> tokens);

  struct
  {
    const std::string name = "painter";

  } config;

  struct
  {
    llama_model *model{};
    llama_context *ctx{};
    llama_batch batch{};

    int n_cur{};
  } llm;
};