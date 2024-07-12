#include <Artifex/clock.h>
#include <Artifex/log.h>
#include <Artifex/mixer.h>

// #include <serial.hpp>
#include <algorithm>
#include <server.hpp>

#include <llm.hpp>
// #include <sd.hpp>

#include <stt.hpp>
#include <tts.hpp>

#include <cstdint>
#include <cstring>
#include <queue>
#include <string>

#define MODEL_LLM "MODEL_PHI3"
#define MODEL_SD "MODEL_SDv2_1"
#define MODEL_WHISPER "MODEL_WHISPER_TINY"
#define MODEL_PIPER "MODEL_PIPER_RYAN"

#define AUDIO_DEVICE NULL

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

// Play Audio Buffer
void play(axMixer mixer, std::vector<int16_t> buffer);

// Sleep
void sleep(float amount);

int main() {
  // Load Models
  LLM llm(getenv(MODEL_LLM), "../prompt.txt");
  // SD sd(getenv(MODEL_SD));

  STT stt(getenv(MODEL_WHISPER));
  TTS tts(getenv(MODEL_PIPER), getenv("ESPEAK_NG_DATA"));

  axMixer mixer;
  axMixerCreate(&mixer, AUDIO_DEVICE);

  ax_verbose("main", "initialization done");

  // Serial serial("/dev/ttyACM0");

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
      } else if (!strncmp(event.data, "PERSON", 6)) {
        if (event.len != 6 + 2) {
          ax_warning("main", "incorrect message size");
          break;
        }

        // serial.send(event.data, event.len);
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

        // memcpy(sd.config.prompt, img->path.c_str(),
        //  std::min(128, (int)img->path.size()));

        // // Configure SD
        // if (!img->data) {
        //   // painting from promp
        //   if (!sd.txt())
        //     continue;
        // } else {
        //   // portrait from image
        //   if (!sd.img())
        //     continue;
        // }

        // TODO send image

        // const char id[5] = {'I', 'M', 'G', 'I', 'N'};
        // uint16_t width = sd.config.width, height = sd.config.height;
        // uint8_t channels = 3;
        // unsigned char data[width * height * channels];

        // server.send(-1, (void *)id, 5 + 2 + 2 + 1 + width * height *
        // channels);
      } else
        sleep(0.3f);
    }
  });

  ax_debug("main", "started painter");

  while (true) {
    // Get User In
    std::string text = stt.listen();
    // printf("You:\n\e[0;92m");
    // char text[256];
    // memset(text, 0, sizeof(text));
    // fgets(text, sizeof(text), stdin);
    // printf("\e[39m");

    if (!strncasecmp(text.c_str(), "headshot", 8))
      break;

    // printf("%s\n", text.c_str());
    printf("%s\n", text.c_str());

    // Generate Response
    printf("AI:\n\e[0;94m");

    // Generate Text
    unsigned int opened = 0;
    std::string message = "", command = "";
    llm.reply(text, [&](std::string token) {
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

    printf("\e[0;39m\n");

    // Generate Audio
    ax_debug("main", "%s", message.c_str());
    play(mixer, tts.say(message));
  }

  play(mixer, tts.say("*gots shot in the head*"));

  ax_debug("main", "cleaning up");

  isRunning = false;
  painter.join();
  axMixerDestroy(&mixer);

  ax_debug("main", "exiting...");

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

void play(axMixer mixer, std::vector<int16_t> buffer) {
  auto id = axMixerLoad(mixer, 1, 22050, buffer.size(), buffer.data());
  axMixerPlay(mixer, id, 1);
  axMixerUnload(mixer, id);
}

void sleep(float amount) {
  float start = axClockNow();
  while (axClockNow() < start + amount)
    ;
}