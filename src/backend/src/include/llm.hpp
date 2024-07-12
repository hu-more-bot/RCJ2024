#pragma once

#include <functional>

#include <string>
#include <vector>

struct LLMdata;

// AI Chat wrapper for the ONNX-GenAI library
class LLM
{
public:
  LLM(const char *model, const char *prompt);
  ~LLM();

  // Generate Reply to User Input
  std::string reply(std::string input,
                    std::function<void(std::string)> onNewToken = {});

  struct
  {
    std::string ai_name = "rick"; //"ai painter";
    std::string user_name = "interviewer";

    unsigned int token_size = 1024;
  } config;

private:
  LLMdata *llm;
};