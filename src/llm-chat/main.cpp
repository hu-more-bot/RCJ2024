#include <llm.hpp>
#include <tts.hpp>
#include <al.hpp>

#include <sd-client.hpp>

// Load Prompt from File
char *loadPrompt(const char *path);

int main()
{
  // Get Model: https://drive.google.com/file/d/1AZNDte9PkblvUFBlB5xSxM9dXoHY1pwM
  // LLM llm("../models/zephyr_q4.gguf", loadPrompt("../prompts/prompt-en.txt"));

  // TTS tts("../models/piper/kusal.onnx");
  // AL al;

  SDClient sd;

  while (true)
  {
  }

  // // while (true)
  // // {
  // //   char user_in[128];
  // //   memset(user_in, 0, sizeof(user_in));
  // //   fgets(user_in, sizeof(user_in), stdin);

  // //   std::string response = llm.reply(user_in);

  // //   auto buf = tts.say(response);
  // //   al.add(buf, 44100 / 2);
  // //   al.play(0);
  // //   al.remove(0);
  // // }

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