#include "log.hpp"
#include <al.hpp>
#include <llm.hpp>
#include <stt.hpp>
#include <tts.hpp>

#include <client.hpp>

#define SPEECH_INPUT false

#define LLM_MODEL "../models/zephyr_q4.gguf"
#define TTS_MODEL "../models/piper/ryan.onnx"
#define STT_MODEL "../models/ggml-tiny.bin"
#define PROMPT "../prompts/en-v4.txt"

char *loadPrompt(const char *path);
std::vector<std::string> parse(std::string &str);

int main() {
  Log::debug("Loading LLM");
  LLM llm(LLM_MODEL, loadPrompt(PROMPT));

  Log::debug("Loading TTS");
  TTS tts(TTS_MODEL);
  AL al;

#if SPEECH_INPUT
  Log::debug("Loading STT");
  STT stt(STT_MODEL);
#endif

  // Connect to SD-Server
  Log::debug("Connecting to SD-Server");
  Client sd(8000);

  // Connect to PoseSim (not necessary)
  Log::debug("Attempting Connection to Pose-Sim");
  Client poseSim(80001);

  Log::debug("Entering Main Loop...");
  while (true) {
    // Wait for user input
    printf("User In: \n");
#if SPEECH_INPUT
    std::string user_in_s = stt.listen();
    char user_in[128];
    printf("Said: '%s'\n", user_in);
#else
    char user_in[128];
    memset(user_in, 0, sizeof(user_in));
    fgets(user_in, sizeof(user_in), stdin);
#endif

    if (!strncasecmp(user_in, "exit", 4))
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
    printf("commands: %zu\n", commands.size());
    for (auto c : commands) {
      char command[8], data[32];
      printf("%s\n", c.c_str());
      sscanf(c.c_str(), "%s: %s", command, data);

      if (!strncmp(command, "SEND", 4))
        sd.send(data);
      else if (!strncmp(command, "POSE", 4))
        poseSim.send(data);
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