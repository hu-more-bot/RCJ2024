#pragma once

#include <Artifex/camera.h>

#include <client.h>
#include <yolo.h>

#include <string.h>

void framecb(struct axCameraFrame *frame, void *user_ptr) {
  yolo_t yolo = (Yolo *)user_ptr;

  if (!frame || !yolo)
    return;

  if (frame->width != 640 || frame->height != 480) {
    printf("invalid frame\n");
    return;
  }

  // Convert YUYV image to RGB
  unsigned char rgb[640 * 640 * 3];
  memcpy(rgb, frame->data, frame->width * frame->height * 3);

  // Fill empty bar
  for (int y = 480; y < 640; y++) {
    for (int x = 0; x < 640; x++) {
      int index = (y * 640 + x) * 3;
      for (int i = 0; i < 3; i++)
        rgb[index + i] = 0;
    }
  }

  // Convert to NCHW
  float data[640 * 640 * 3];

  int i = 0;
  for (int y = 0; y < 640; y++) {
    for (int x = 0; x < 640; x++) {
      int index = (y * 640 + x) * 3;
      data[0 * 640 * 640 + i] = rgb[index + 0] / 255.0f;
      data[1 * 640 * 640 + i] = rgb[index + 1] / 255.0f;
      data[2 * 640 * 640 + i] = rgb[index + 2] / 255.0f;
      i++;
    }
  }

  // Run Inference
  struct Result *result;
  int count = yoloDetect(yolo, data, &result);

  float max = 0;
  for (int i = 0; i < count; i++) {
    float x = result[i].x, y = result[i].y, w = result[i].w, h = result[i].h;
    float area = w * h;

    if (result[i].index == 0) { // person
      if (area > max) {
        max = area;
        person = (640 - x) / 640.0f - w / 640.0f;
      }
    }
  }

  free(result);

  // int max = 0;
  // x = 0, y = 0, w = 0, h = 0;
  // for (auto d : detections) {
  //   for (auto dd : d) {
  //     if (dd.name == "person") {
  //       int size = dd.w * dd.h;
  //       if (size > max) {
  //         x = (640 - dd.x) / 640.0f, y = (480 - dd.y) / 480.0f,
  //         w = dd.w / 640.0f, h = dd.h / 480.0f;
  //         max = size;
  //       }
  //       // printf("%f %f\n", dd.x, dd.y);
  //     }
  //     // printf("%s\n", dd.name.c_str());
  //   }

  // printf("\n\n");
}
//     // Generate Response Audio
//     auto buf = tts.say(response);
//     al.add(buf, 44100 / 2);

//     // Process Commands
//     printf("commands: %zu\n", commands.size());
//     for (auto c : commands) {
//       // printf("%s\n", c.c_str());
//       sd.send(c);
//       // poseSim.send("idle");
void clientcb(const struct clientEvent *event, void *user_ptr) {
  switch (event->type) {
  case clientEvent::CONNECTION:
    break;

  case clientEvent::DISCONNECTION:
    break;

  case clientEvent::TIMEOUT:
    break;

  case clientEvent::MESSAGE: {
    // SPK: raw generated text with emotes embedded
    // IMG: generated image to get displayed
    if (strncmp("TXT", event->data, 3)) {
      // TODO
    }
  } break;
  }
}