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
#define MODEL_WHISPER "MODEL_WHISPER_BASE"
#define MODEL_PIPER "MODEL_PIPER_HFC_MALE"

#define AUDIO_DEVICE NULL

volatile bool isRunning = true;

struct Image
{
  unsigned char *data;
  uint16_t width, height;
  uint8_t channels;

  std::string path;
};

void sleep(float amount);

int main()
{
  // Load Models
  LLM llm(getenv(MODEL_LLM), "../prompt.txt");
  // SD sd(getenv(MODEL_SD));

  STT stt(getenv(MODEL_WHISPER));
  TTS tts(getenv(MODEL_PIPER), getenv("ESPEAK_NG_DATA"));

  axMixer mixer;
  axMixerCreate(&mixer, AUDIO_DEVICE);

  ax_verbose("main", "initialization done");

  // Serial serial("/dev/ttyACM0");

  // Image Queue
  std::queue<Image *> image;

  // Start Server
  Server server(8000, [&](Server &server, const Server::Event &event)
                {
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
    } });

  ax_debug("main", "started server");

  std::thread painter([&]
                      {
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
    } });

  ax_debug("main", "started painter");

  while (true)
  {
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

    // Lambda to process command
    auto process = [&](const char *command)
    {
      if (!strncmp(command, "PAINT", 5))
      {
        // Add image prompt to queue
        Image *img = new Image;
        img->data = NULL;
        img->width = img->height = 0;
        img->channels = 0;
        img->path = std::string(command + 7);
        image.push(img);
        ax_debug("main", "added to image queue (new size: %zu)", image.size());
      }
      else if (!strncmp(command, "PORTRAIT", 8))
      {
        //
      }
      else
      {
        //
      }
    };

    // Generate Text
    unsigned int opened = 0;
    std::string message, command;

    llm.reply(text, [&](std::string token)
              {
      // parse commands
      if (token[0] == '[')
        opened++;
      else if (token[0] == ']') {
        if (opened > 0)
          opened--;
        else
          return;

        if (opened == 0) {
          process(command.c_str());
          command = "";
        }
      } else {
        if (opened > 0)
          command += token;
        else
          message += token;
      }
      // TODO fix
      printf("%s", token.c_str());
      fflush(stdout); });
    printf("\e[0;39m\n");

    // Generate Audio
    ax_debug("main", "%s", message.c_str());
    auto buf = tts.say(message);

    // Play Audio
    auto id = axMixerLoad(mixer, 1, 22050, buf.size(), buf.data());
    axMixerPlay(mixer, id, 1);
    axMixerUnload(mixer, id);
  }

  auto buf = tts.say("*gots shot in the head*");

  // Play Audio
  auto id = axMixerLoad(mixer, 1, 22050, buf.size(), buf.data());
  axMixerPlay(mixer, id, 1);
  axMixerUnload(mixer, id);

  ax_debug("main", "cleaning up");

  isRunning = false;
  painter.join();
  axMixerDestroy(&mixer);

  ax_debug("main", "exiting...");

  assert(0);

  return 0;
}

void sleep(float amount)
{
  float start = axClockNow();
  while (axClockNow() < start + amount)
    ;
}