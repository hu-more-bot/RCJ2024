#pragma once

#include <session.h>

#include <stdlib.h>
#include <string.h>

#include <stdint.h>

#include "Artifex/log.h"
#include "image.h"

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define LIMIT(x, min, max) (MAX(MIN(x, max), min))
#define PERCENT(x) LIMIT(x, 0, 1)

// Output Image
#define IM_RES_W 274
#define IM_RES_H 480

void framecb(struct axCameraFrame *frame, void *user_ptr) {
  struct Session *session = (struct Session *)user_ptr;

  if (!frame || !session)
    return;

  if (frame->width != 640 || frame->height != 480) {
    ax_warning("camera", "invalid frame");
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
  int count = yoloDetect(session->yolo, data, &result);

  float max = 0;
  int id = -1;
  float pos[2], size[2];
  for (int i = 0; i < count; i++) {
    float w = result[i].w, h = result[i].h;
    float x = result[i].x, y = result[i].y;
    float area = w * h;

    if (result[i].index == 0) {
      if (area > max) {
        max = area;
        id = i;
        pos[0] = x, pos[1] = y;
        size[0] = w, size[1] = h;
      }
    }
  }

  // TODO if has person & im req; crop & save im
  if (id >= 0) {
    // set relative position
    session->person =
        PERCENT((pos[0] + size[0] / 2.0f) / frame->width) * -2.0f + 1;

    const char id[5] = "IMGIN";
    uint16_t width = IM_RES_W, height = IM_RES_H;
    uint8_t channels = 3;
    unsigned char cropped[IM_RES_W * IM_RES_H * 3];

    // crop image
    cropImage(frame->data, frame->width, frame->height, cropped, IM_RES_W,
              IM_RES_H, pos[0] + size[0] / 2.0f, 320);

    // send image data [id, w, h, ch, data]
    clientSend(session->client, (char *)&width,
               5 + 2 + 2 + 1 + IM_RES_W * IM_RES_H * 3);

    session->image.request = 0;
  }

  free(result);
}

void clientcb(const struct clientEvent *event, void *user_ptr) {
  struct Session *session = user_ptr;

  switch (event->type) {
  case MESSAGE: {
    // SPK: raw generated text with emotes embedded
    // IMG: generated image to get displayed
    if (!strncmp("IMREQ", event->data, 5)) {
      session->image.request = 1; // request image
      ax_debug("main", "image request set");
    } else if (!strncmp("IMGIN", event->data, 5)) {
      const char id[5] = "IMGIN";

      memcpy(&session->image.width, &event->data[5], 2);
      memcpy(&session->image.height, &event->data[7], 2);
      memcpy(&session->image.channels, &event->data[9], 1);

      unsigned long size = session->image.width * session->image.height *
                           session->image.channels;
      if (event->len != 5 + 2 + 2 + 1 + size) {
        ax_warning("main", "incorrect message size");
        break;
      }

      free(session->image.data);
      session->image.data = malloc(size);
      memcpy(&session->image.data, &event->data[10], size);

      session->image.refresh = 1;
      ax_debug("main", "image refresh required");
    } else
      ax_warning("main", "incorrect message header");
  } break;

  default:
    break;
  }
}