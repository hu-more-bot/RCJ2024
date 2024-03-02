#include <llm.hpp>
#include <sd.hpp>
#include <tts.hpp>

#include <thread>
#include <string.h>

// Load Prompt from File
char *loadPrompt(const char *path);

int main()
{
  // Renderer renderer("asd");

  // std::thread render(SD::txt2img, "../models/...", "prompt");

  // Get Model: https://drive.google.com/file/d/1AZNDte9PkblvUFBlB5xSxM9dXoHY1pwM
  LLM llm("../models/zephyr_q4.gguf", loadPrompt("../prompts/prompt-en.txt"));

  TTS tts("../models/piper/kusal.onnx");

  while (true)
  {
    char user_in[128];
    memset(user_in, 0, sizeof(user_in));
    fgets(user_in, sizeof(user_in), stdin);

    std::string response = llm.reply(user_in);

    auto buf = tts.say(response);
  }

  // while (renderer.Window::update())
  // {
  //   if (renderer.key("esc"))
  //     renderer.exit();

  //   if (render.joinable())
  //     renderer.draw();
  // }

  // while (!render.joinable())
  // ;

  // render.detach();

  return 0;
}

char *loadPrompt(const char *path)
{
  // Open File
  FILE *f = NULL;
  if (!(f = fopen(path, "r")))
  {
    printf("Failed to open file: %s\n", path);
    return NULL;
  }

  // Get File Size
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, 0, SEEK_SET);

  // Read File
  char *prompt = (char *)malloc(sizeof(char) * size);

  if (!fread(prompt, size, 1, f))
  {
    printf("Failed to read file: %s\n", path);
    free(prompt);
    return NULL;
  }

  return prompt;
}