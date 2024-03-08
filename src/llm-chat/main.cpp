#include <al.hpp>
#include <llm.hpp>
#include <stt.hpp>
#include <tts.hpp>

#include <sd-client.hpp>

#define LLM_MODEL "../models/zephyr_q4.gguf"
#define TTS_MODEL "../models/piper/ryan.onnx"
#define STT_MODEL "../models/ggml-tiny.bin"
#define PROMPT "../prompts/en-v3.txt"

char *loadPrompt(const char *path);
std::vector<std::string> parse(std::string &str);

int main() {
  printf("Loading LLM...\n");
  LLM llm(LLM_MODEL, loadPrompt(PROMPT));

  printf("Loading TTS...\n");
  TTS tts(TTS_MODEL);
  AL al;

  printf("Loading STT...\n");
  STT stt(STT_MODEL);

  printf("Starting SDClient...\n");
  SDClient sd;

  printf("Enterint Main Loop...\n");
  while (true) {
    // Wait for user input
    printf("User In: \n");
    // char user_in[128];
    // memset(user_in, 0, sizeof(user_in));
    // fgets(user_in, sizeof(user_in), stdin);

    std::string user_in = stt.listen();
    printf("Said: '%s'\n", user_in.c_str());

    if (!strncasecmp(user_in.c_str(), "exit", 4))
      break;

    // Generate Response
    std::string response = llm.reply(user_in);
    auto commands = parse(response);
    printf("Response: %s", response.c_str());

    // Handle 0-length responses
    if (response.size() == 0)
      response = "One minute please...";

    // Generate Response Audio
    auto buf = tts.say(response);
    al.add(buf, 44100 / 2);

    // Process Commands
    printf("commands: %i\n", commands.size());
    for (auto c : commands) {
      // printf("%s\n", c.c_str());
      sd.send(c);
    }

    // Play Response Audio
    al.play(0);
    al.remove(0);
  }

  return 0;
}

/**
 * Load Prompt from file
 *
 * @param path path to prompt (text) file
 * @returns loaded prompt
 */
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

/**
 * Separate substrings (separated by "[]") from str
 *
 * @param str unformatted string reference, removes substrings
 * @returns list of substrings
 */
std::vector<std::string> parse(std::string &str) {
  std::string clean;
  std::vector<std::string> out;

  int isEnclosed = 0;
  std::string parsed;
  for (auto c : str) {
    if (c == '[') {
      isEnclosed++;
    } else if (c == ']') {
      isEnclosed--;

      if (isEnclosed == 0) {
        out.push_back(parsed);
        parsed.clear();
      }
    } else {
      if (!isEnclosed)
        clean += c;
      else
        parsed += c;
    }
  }

  str = clean;
  return out;
}