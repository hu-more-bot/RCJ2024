#include <Artifex/clock.h>
#include <Artifex/log.h>
#include <Artifex/mixer.h>

// #include <serial.hpp>
#include <algorithm>
#include <server.hpp>

#include <llm.hpp>
#include <sd.hpp>

#include <stt.hpp>
#include <tts.hpp>

#include <cstdint>
#include <cstring>
#include <queue>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Model Config
#define MODEL_LLM "MODEL_PHI3"
#define MODEL_SD "MODEL_SDv2_1"
#define MODEL_WHISPER "MODEL_WHISPER_BASE"
#define MODEL_PIPER "MODEL_PIPER_RYAN"

// Other Config
#define AUDIO_DEVICE NULL
#define USE_STT true

volatile bool isRunning = true;

struct Image {
  unsigned char *data;
  uint16_t width, height;
  uint8_t channels;

  std::string path;
};

typedef std::queue<Image *> ImageQueue;

// Process Command
void process(ImageQueue &img, Server &server, const char *command);

// Sleep
void sleep(float amount);

int main() {
  Server s(8000, [&](Server &server, const Server::Event &event) {
    if (event.type == Server::Event::MESSAGE)
      printf("%.*s\n", (int)event.len, event.data);
  });

  while (1)
    ;

  // Load Models
  LLM llm(getenv(MODEL_LLM), "../prompt.txt");
  SD sd(getenv(MODEL_SD));
  // sd.config.width = 256;
  // sd.config.height = 256;

#if USE_STT
  STT stt(getenv(MODEL_WHISPER));
#endif
  TTS tts(getenv(MODEL_PIPER), getenv("ESPEAK_NG_DATA"));

  axMixer mixer;
  axMixerCreate(&mixer, AUDIO_DEVICE);

  ax_verbose("main", "initialization done");

  ImageQueue image;

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
      }
    } break;

    default:
      break;
    }
  });

  ax_debug("main", "started server");

  std::thread painter([&] {
    Image *img = NULL;
    while (isRunning) {
      // free previous image
      if (img) {
        free(img);
        img = NULL;
      }

      // generate image
      if (!image.empty()) {
        img = image.front();
        image.pop();

        ax_debug("main", "removed image from queue (new size: %zu)",
                 image.size());

        if (img->path.empty())
          continue;

        memcpy(sd.config.prompt, img->path.c_str(),
               std::min(128, (int)img->path.size()));

        // Generate Image
        if (!img->data) {
          // painting from promp
          if (!sd.txt())
            continue;
        } else {
          // portrait from image
          if (!sd.img())
            continue;
        }

        // return if no img
        if (!sd.result)
          continue;

        const char id[5] = {'I', 'M', 'G', 'I', 'N'};
        uint16_t width = sd.result->width, height = sd.result->height;
        uint8_t channels = sd.result->channel;
        unsigned char data[width * height * channels];

        // send to client(s)
        server.send(-1, (void *)id, 5 + 2 + 2 + 1 + width * height * channels);

        // save as png
        stbi_write_png((std::to_string(time(0)) + ".png").c_str(), width,
                       height, channels, data, width * channels);
      } else
        sleep(0.3f);
    }
  });

  ax_debug("main", "started painter");

  while (true) {
    // Get User In
    std::string text;
#if USE_STT
    text = stt.listen();
#elif
    printf("You:\n\e[0;92m");
    char userin[256];
    memset(userin, 0, sizeof(userin));
    fgets(userin, sizeof(userin), stdin);
    printf("\e[39m");
    text = userin;
#endif

    if (!strncasecmp(text.c_str(), "headshot", 8) ||
        !strncasecmp(text.c_str(), "bang", 4) ||
        !strncasecmp(text.c_str(), "(gunshots)", 11))
      break;

    // printf("%s\n", text.c_str());
    printf("%s\n", text.c_str());

    std::string message = "";
    /* Generate Response */ {
      printf("Generating Response...\n");

      // Generate Text
      unsigned int opened = 0;
      std::string command = "";
      llm.reply(text, [&](std::string token) {
        printf("%s", token.c_str());
        fflush(stdout);
        // parse tokens -> message & command(s)
        for (char c : token) {
          if (c == '[') {
            opened++;
          } else if (c == ']') {
            if (opened > 0)
              opened--;
            else
              continue;

            if (opened == 0) {
              // process command
              process(image, server, command.c_str());
              command = "";
            }
          } else
            (opened ? command : message) += c;
        }
      });
    }

    /* Generate Audio */ {
      auto buffer = tts.say(message);
      auto id = axMixerLoad(mixer, 1, 22050, buffer.size(), buffer.data());

      /* Send Text */ {
        char id[4] = {'T', 'E', 'X', 'T'};
        float alen = buffer.size() / 22050.0f;
        const uint16_t len = message.size();
        char data[len];

        memcpy(data, message.data(), len);
        server.send(-1, id,
                    sizeof(id) + sizeof(float) + sizeof(uint16_t) + len);
      }

      axMixerPlay(mixer, id, 1);
      axMixerUnload(mixer, id);
    }
  }

  ax_debug("main", "cleaning up");

  /* Say sayonara */ {
    const char *messages[] = {"*gots shot in the head*",
                              "sayonara",
                              "it was a pleasure painting with you",
                              "*shoots back at you*",
                              "haha, you've missed. maybe next time",
                              "see you soon, space cowboy",
                              "you got that skibidi sigma rizz",
                              "why would you do that?",
                              "wow that was pretty cool"};
    const unsigned long len = sizeof(messages) / sizeof(*messages);

    auto buffer = tts.say(messages[rand() % len]);
    auto id = axMixerLoad(mixer, 1, 22050, buffer.size(), buffer.data());
    axMixerPlay(mixer, id, 1);
    axMixerUnload(mixer, id);
  }

  isRunning = false;
  painter.join();
  axMixerDestroy(&mixer);

  ax_debug("main", "exiting...");

  // sdl2 messes up the STT destructor
  // temporary fix, but works
  assert(0);

  return 0;
}

void process(ImageQueue &image, Server &server, const char *command) {
  if (!strncmp(command, "PAINT", 5)) {
    // Add image prompt to queue
    Image *img = new Image;
    img->data = NULL;
    img->width = img->height = 0;
    img->channels = 0;
    img->path = std::string(command + 7);
    image.push(img);
    ax_debug("main", "added to image queue (new size: %zu)", image.size());
  } else if (!strncmp(command, "PORTRAIT", 8)) {
    // Send Image Request
    const char message[5] = {'I', 'M', 'R', 'E', 'Q'};
    server.send(-1, (void *)message, sizeof(message));
    // TODO send prompt or smthing
  } else
    ax_warning("main", "unknown command");
}

void play(axMixer mixer, std::vector<int16_t> buffer) {}

void sleep(float amount) {
  float start = axClockNow();
  while (axClockNow() < start + amount)
    ;
}