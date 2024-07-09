#include <Artifex/clock.h>
#include <Artifex/log.h>

#include <serial.hpp>
#include <server.hpp>

#include <llm.hpp>
#include <sd.hpp>

#include <stt.hpp>
#include <tts.hpp>

#include <queue>

struct Image {
  unsigned char *data;
  uint16_t width, height;
  uint8_t channels;

  std::string path;
};

void sleep(float amount);

int main() {
  // Load Models
  LLM llm("../models/zephyr-q4.gguf", "../prompt.txt");
  SD sd("../models/sd.gguf");

  TTS tts("../models/piper/ryan.onnx");
  STT stt("../models/whisper/ggml-base.en-q5_1.bin");

  ax_verbose("main", "initialization done");

  Serial serial("/dev/ttyACM0");

  // Image Queue
  std::queue<Image *> image;

  // Start Server
  Server server(8000, [&](Server &server, const Server::Event &event) {
    switch (event.type) {
    case Server::Event::MESSAGE: {
      if (!strncmp(event.data, "IMAGE", 5)) {
        if (event.len <= 5 + 2 + 2 + 1) {
          ax_warning("main", "incorrect message size");
          break;
        }

        Image *img = new Image();

        memcpy(&img->width, &event.data[5], 2);
        memcpy(&img->height, &event.data[7], 2);
        memcpy(&img->channels, &event.data[9], 1);

        unsigned long size = img->width * img->height * img->channels;

        if (event.len != 5 + 2 + 2 + 1 + size) {
          ax_warning("main", "incorrect message size");
          free(img);
          break;
        }

        img->data = new unsigned char[size];
        memcpy(img->data, &event.data[10], size);

        image.push(img);
      } else if (!strncmp(event.data, "PERSON", 6)) {
        if (event.len != 6 + 2) {
          ax_warning("main", "incorrect message size");
          break;
        }

        serial.send(event.data, event.len);
      }
    } break;

    default:
      break;
    }
  });

  ax_debug("main", "started server");

  std::thread painter([&] {
    if (!image.empty()) {
      // Start Processing Image
      Image *img = image.front();
      image.pop();

      // TODO generate & send image

      const char id[5] = {'I', 'M', 'A', 'G', 'E'};
      uint16_t width = sd.config.width, height = sd.config.height;
      uint8_t channels = 3;
      unsigned char data[width * height * channels];

      server.send(-1, (void *)id, 5 + 2 + 2 + 1 + width * height * channels);
    } else
      sleep(0.3f);
  });
  ax_debug("main", "started painter");

  while (true) {
    // Get User In
    printf("You:\n\e[0;92m");
    char text[256];
    memset(text, 0, sizeof(text));
    fgets(text, sizeof(text), stdin);
    printf("\e[39m");

    // Generate Response
    printf("AI:\n\e[0;94m");
    llm.decode("client", text);
    llm.generate([&](std::string token) {
      // TODO action parser
      printf("%s", token.c_str());
      fflush(stdout);
    });
    printf("\e[0;39m");
  }
}

void sleep(float amount) {
  float start = axClockNow();
  while (axClockNow() < start + amount)
    ;
}