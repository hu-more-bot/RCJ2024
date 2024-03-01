#include <renderer.hpp>

#include <llm.hpp>
#include <sd.hpp>

#include <thread>

const struct {
  const char *llm = "../models/zephyr_q4.gguf";
  const char *prompt = "../llm-prompt.txt";

  const char *sd = "../models/sd-v1-4.ckpt";

  // const char *ss = "../models/whisper...";
  // const char *tts = "../models/piper...";
} PATH;

// Load Prompt from File
char *loadPrompt(const char *path);

// Generate Image
void sd_draw(std::string prompt) { SD::txt2img(PATH.sd, prompt); }

int main() {
  Renderer renderer("asd");

  // std::thread render(sd_draw, "dog");

  LLM llm(PATH.llm, loadPrompt(PATH.prompt));

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

char *loadPrompt(const char *path) {
  // Open File
  FILE *f = NULL;
  if (!(f = fopen(path, "r"))) {
    printf("Failed to open file: %s\n", path);
    return NULL;
  }

  // Get File Size
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, 0, SEEK_SET);

  // Read File
  char *prompt = (char *)malloc(sizeof(char) * size);

  if (!fread(prompt, size, 1, f)) {
    printf("Failed to read file: %s\n", path);
    free(prompt);
    return NULL;
  }

  return prompt;
}