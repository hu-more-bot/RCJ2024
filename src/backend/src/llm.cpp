#include <Artifex/log.h>

#include <llm.hpp>

#include <stdexcept>

#include <ort_genai.h>

#define TAG "llm"

struct LLMdata
{
  OgaHandle handle;

  std::unique_ptr<OgaModel> model;
  std::unique_ptr<OgaTokenizer> tokenizer;
  std::unique_ptr<OgaTokenizerStream> tokenizer_stream;

  std::string text;
};

LLM::LLM(const char *modelPath, const char *promptPath)
{
  if (!(llm = new LLMdata))
  {
    ax_error(TAG, "failed to allocate memory");
    return;
  }

  llm->model = OgaModel::Create(modelPath);

  if (!llm->model)
  {
    ax_error(TAG, "failed to create model");
    return;
  }

  llm->tokenizer = OgaTokenizer::Create(*llm->model);

  if (!llm->tokenizer)
  {
    ax_error(TAG, "failed to create tokenizer");
    return;
  }

  llm->tokenizer_stream = OgaTokenizerStream::Create(*llm->tokenizer);

  if (!llm->tokenizer_stream)
  {
    ax_error(TAG, "failed to create tokenizer stream");
    return;
  }

  char *prompt = NULL;
  /* Load Prompt */ {
    // Open File
    FILE *f = NULL;
    if (!(f = fopen(promptPath, "r")))
    {
      ax_error(TAG, "failed to open file");
      return;
    }

    // Get File Size
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Read File
    prompt = (char *)malloc(sizeof(char) * size);

    if (!fread(prompt, size, 1, f))
    {
      ax_error(TAG, "failed to read prompt");
      return;
    }
  }

  llm->text = "<|instruction|>\n" + std::string(prompt) + "<|end|>\n";

  ax_verbose(TAG, "initialized");
}

LLM::~LLM() { delete llm; }

std::string LLM::reply(std::string input,
                       std::function<void(std::string)> onNewToken)
{
  std::string out;

  llm->text += "<|" + config.user_name + "|>\n" + input + "<|end|>\n<|" +
               config.ai_name + "|>";

  auto sequences = OgaSequences::Create();
  llm->tokenizer->Encode(llm->text.c_str(), *sequences);

  auto params = OgaGeneratorParams::Create(*llm->model);
  params->SetSearchOption("max_length", config.token_size);
  params->SetInputSequences(*sequences);

  auto generator = OgaGenerator::Create(*llm->model, *params); // asd

  while (!generator->IsDone())
  {
    generator->ComputeLogits();
    generator->GenerateNextToken();

    const auto num_tokens = generator->GetSequenceCount(0);
    const auto new_token = generator->GetSequenceData(0)[num_tokens - 1];
    std::string token = llm->tokenizer_stream->Decode(new_token);

    llm->text += token, out += token;
    onNewToken(token);
  }

  llm->text += "<|end|>\n";

  return out;
}