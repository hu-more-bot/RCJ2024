#include <server.h>

#include <llm.hpp>
#include <sd.hpp>

#include <Artifex/log.h>

#include <list>
#include <thread>

#include "callback.hpp"

int main() {
  LLM llm("../models/zephyr-7b-beta.gguf", "../prompt.txt");
  ax_verbose("main", "loaded llm");

  SD sd("../models/sd.gguf");
  ax_verbose("main", "loaded sd");

  std::thread painter; // image generator thread

  server_t server;
  serverCreate(&server, 8000);

  ax_verbose("main", "initialization done");

  std::list<std::string> pool;

  serverStart(server, servercb, NULL);

  while (true) {
    // Get User In
    printf("You:\n\e[0;92m");
    char text[256];
    memset(text, 0, sizeof(text));
    fgets(text, sizeof(text), stdin);
    printf("\e[39m");

    llm.decode("client", std::string(text));

    // Print AI Out
    std::vector<std::string> response(1, "");
    size_t cursor = 0;

    std::string out = "", cmd;
    bool open = false;
    llm.generate([&](std::string token) {
      if (open) {
        if (token == "]") { // close
          open = false;

          printf("command: `%s`\n", cmd.c_str());

          // TODO process command
        } else
          cmd += token;
      } else {
        if (token == "[") { // open
          open = true;
          cmd = "";

          if (!response[cursor].empty()) {
            response.push_back("");
            cursor++;
          }
        } else
          response[cursor] += token;
      }
    });

    printf("response: %zu", cursor + 1);
  }
}