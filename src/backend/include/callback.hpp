#pragma once

#include <llm.hpp>
#include <server.h>

#include <string.h>

// TCP Server Callback
void servercb(const struct serverEvent *event, void *user_ptr) {
  if (event->type != serverEvent::MESSAGE)
    return;

  if (!strncmp(event->data, "TXIN", 4)) {
    // Decode Message & Generate Response
    llm.decode("client", std::string(event->data + 4, event->len));
  } else if (!strncmp(event->data, "IMTX", 4)) {
    // Generate Image from prompt

    struct {
      int width;
      int height;
      const char *prompt;
    } msg;

    memcpy(&msg, event->data + 4, sizeof(int) * 2);
    msg.prompt = event->data + 4 + sizeof(int) * 2;

    // Start Painter Thread
    painter = (std::thread)[&] {
      sd.config.width = msg.width;
      sd.config.height = msg.height;
      sd.config.seed = time(0);

      strcpy(sd.config.prompt, msg.prompt);

      if (!sd.generate()) {
        printf("failed to generate image\n");
      }

      // TODO compress & send image
      // format: "IM[ch][size][IMAGE DATA]"
      char out[2 + sizeof(int) * 3 +
               sd.result->channel * sd.result->width * sd.result->height];
      memcpy(out, "IM", 2);
      memcpy(out + 2, &sd.result->channel, sizeof(int));
      memcpy(out + 2 + sizeof(int), &sd.result->width, sizeof(int));
      memcpy(out + 2 + sizeof(int) * 2, &sd.result->height, sizeof(int));

      server.send(event.sockfd, out);
    };
  } else if (!strncmp(event->data, "IMIN", 4)) {
    // Generate Image from prompt using image
  }
}