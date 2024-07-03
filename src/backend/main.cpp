#include <Artifex/clock.h>
#include <Artifex/log.h>

#include <server.hpp>

#include <llm.hpp>
#include <sd.hpp>

#include <queue>

struct Image {
  unsigned char *data;
  uint16_t width, height;
  uint8_t channels;
};

void sleep(float amount);

int main() {
  LLM llm("../models/zephyr-q4.gguf", "../prompt.txt");
  SD sd("../models/sd.gguf");

  ax_verbose("main", "initialization done");

  struct {
    std::queue<std::string> text;
    std::queue<Image *> image;
  } queue;

  std::thread painter([&] {
    if (!queue.image.empty()) {
      // Start Processing Image
      Image *img = queue.image.front();
      queue.image.pop();

      // TODO generate & send image
    }
  });

  std::thread speaker([&] {
    if (!queue.text.empty()) {
      // Decode Text
      std::string text = queue.text.front();
      queue.text.pop();

      llm.decode("interviewer", text);
    } else {
      // Generate Response
      std::vector<std::string> response(1, "");
      size_t cursor = 0;

      std::string cmd;
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
    }

    if (queue.text.empty())
      sleep(0.1);
  });

  Server server(8000, [&](Server &server, const Server::Event &event) {
    switch (event.type) {
    case Server::Event::MESSAGE:
      if (!strncmp(event.data, "TXTIN", 5)) {
        queue.text.push(std::string(event.data, event.len));
      } else if (!strncmp(event.data, "IMGIN", 5)) {
        Image *img = new Image();

        memcpy(&img->width, &event.data[5], 2);
        memcpy(&img->height, &event.data[7], 2);
        memcpy(&img->channels, &event.data[9], 1);

        unsigned long size = img->width * img->height * img->channels;

        if (event.len != 5 + 2 + 2 + 1 + size) {
          // invalid size
          // TODO
        }

        img->data = new unsigned char[size];
        memcpy(img->data, &event.data[10], size);

        queue.image.push(img);
      } else if (!strncmp(event.data, "PERSON", 6)) { // TODO shorter name
        // TODO person position
        // TODO forward to pico
      }
      break;

    default:
      break;
    }
  });
}

void sleep(float amount) {
  float start = axClockNow();
  while (axClockNow() < start + amount)
    ;
}