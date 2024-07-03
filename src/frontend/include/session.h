#pragma once

#include <Artifex/log.h>

#include <Artifex/camera.h>
#include <Artifex/renderer.h>

#include <client.h>
#include <yolo.h>

#include <stdint.h>

struct Session {
  axCamera camera;
  yolo_t yolo;

  client_t client;

  axWindow window[2];
  axRenderer renderer[2];

  float person;

  struct {
  volatile int request;
  volatile int refresh;

  uint16_t width, height;
  uint8_t channels;
  unsigned char *data;
  } image;
};