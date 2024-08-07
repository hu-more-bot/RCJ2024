#pragma once

#include <session.h>

#include <Artifex/clock.h>

#include <math.h>

#include "Artifex/log.h"
#include "Artifex/renderer.h"
#include "Artifex/vec.h"
#include "callbacks.h"
#include "image.h"

#define ADDR "10.42.0.1"
#define PORT 8000

// Render Face with renderer while looking at lookat
// [lookat: -1 left, 0 center, 1 right]
// [state: idle (tracking), 'yes', 'no']
void renderFace(axRenderer renderer, float lookat, uint8_t state) {
  const axVector size = {0.23f, 0.3f};
  const float movement = 0.1f;
  const float scale = 0.3f;
  const float round = 0.8f;
  const float offset = 0.5f;

  static float frameStart = -1;

  // Timing
  static float past = 0;
  static float now = 0;
  static float delta = 0;

  past = now;
  now = axClockNow();
  delta = now - past;

  static axVector prev, center;
  static axVector target = {};

  if (frameStart < 0)
    frameStart = axClockNow();
  else {
    // TODO process frame
  }

  struct axRendererDrawInfo drawInfo = {};
  drawInfo.style = 0;
  drawInfo.corner[0] = drawInfo.corner[1] = drawInfo.corner[2] =
      drawInfo.corner[3] = round;

  static int aa;
  switch (state) {
  default:
  case 0: // idle (tracking)
    // if ((int)axClockNow() % 3 == 0 && (int)axClockNow() != aa) {
    // aa = axClockNow();
    // center.x = center.x <= 0 ? 1 : -1;
    // }
    center.x = lookat;
    break;
  }

  // Smoothing

  // Smooth out movement
  const float s = 0.8 * delta;
  // pwm = (pwm * s_amount) + (prev * (100% - s_amount))
  axVector value;
  float squish = 0;
  value.x = LIMIT((center.x * s) + (prev.x * (1.0 - s)), -0.2f, 0.2f);
  value.y = (center.y * s) + (prev.y * (1.0 - s));
  squish = value.x + prev.x;
  prev.x = value.x, prev.y = value.y;
  drawInfo.size = size;

  // Eyes
  drawInfo.center.x = value.x + offset;
  axRendererDraw(renderer, &drawInfo);
  drawInfo.center.x = value.x - offset;
  axRendererDraw(renderer, &drawInfo);
}

// Process & Render UI
void renderUI(axWindow window, axRenderer renderer, unsigned int textures[7],
              struct Session *session) {
  static int prevState = 0;
  static float state = 0;
  static int touchState = 0;
  static float touchOffset = 0;
  static float last, now;
  last = now;
  now = axClockNow();
  float delta = now - last;

  axVector cursor;
  axWindowGetCursor(window, &cursor.x, &cursor.y);
  cursor.y /= axWindowRatio(window);

  if (axWindowGetKeyState(window, AX_WINDOW_KEY_MOUSE_LEFT)) {
    if (touchState == 0) {
      touchState = 1;
      touchOffset = -cursor.y - state;
    } else {
      state = -cursor.y - touchOffset;
    }
  } else {
    touchState = 0;
    // Snap to pos
    if (state > 0 && state < (prevState == 0 ? 0.2f : 0.8f)) {
      state -= 4.0f * delta;
    } else if (state < 1 && state > (prevState == 0 ? 0.2f : 0.8f)) {
      state += 4.0f * delta;
    }
  }

  if (state < 0) {
    prevState = 0;
    state = 0;
  }
  if (state > 1) {
    prevState = 1;
    state = 1;
  }

  struct axRendererDrawInfo drawInfo = {};
  drawInfo.style = 1;
  drawInfo.color = (axColor){0.2f, 0.2f, 0.2f};
  drawInfo.corner[0] = drawInfo.corner[1] = 0.2f;
  drawInfo.size.x = 1.0f;
  drawInfo.size.y = 0.5f;
  drawInfo.center.y = (2.75f) / axWindowRatio(window) - state;

  axRendererDraw(renderer, &drawInfo);

  static int prevButtonState = 0;
  static int isRunning = 0;

  for (int x = 0; x < 4; x++) {
    struct axRendererDrawInfo drawInfo = {};
    drawInfo.style = 0;
    drawInfo.corner[0] = drawInfo.corner[1] = drawInfo.corner[2] =
        drawInfo.corner[3] = 2.0f;
    drawInfo.size.x = 0.1;
    drawInfo.size.y = 0.1f;
    drawInfo.center.x = ((2.0f / 4.0f) * (x - 1.5));
    drawInfo.center.y = (2.25f) / axWindowRatio(window) - state;

    // TODO maybe remove background
    // axRendererDraw(renderer, &drawInfo);

    static int buttonState[4] = {};
    // buttonState[1] = clientIsConnected(session->client);
    buttonState[2] = 0;
    buttonState[3] = axWindowIsFullscreen(window);

    drawInfo.style = 2;
    drawInfo.texture = textures[x + (buttonState[x] ? 3 : 0)];
    axRendererDraw(renderer, &drawInfo);

    if (!axWindowGetKeyState(window, AX_WINDOW_KEY_MOUSE_LEFT) &&
        prevButtonState) {
      // Check bounding box
      if (drawInfo.center.x - drawInfo.size.x < cursor.x &&
          cursor.x < drawInfo.center.x + drawInfo.size.x) {
        if (drawInfo.center.y - drawInfo.size.y < cursor.y &&
            cursor.y < drawInfo.center.y + drawInfo.size.y) {
          switch (x) {
          case 0: {
            axWindowExit(window);
          } break;

          case 1: {
            // if (!buttonState[1])
            // clientOpen(session->client, ADDR, PORT);
            // else
            // clientClose(session->client);
          } break;

          case 2: {
            // if (!isRunning) {
            //   // Start Streams
            //   if (clientStartListening(session->client, clientcb, NULL))
            //     break;

            //   // TODO start audio stream
            //   if (axCameraStart(session->camera, framecb, session)) {
            //     clientStopListening(session->client);
            //     break;
            //   }

            //   isRunning = 1;
            // } else {
            //   // Stop Streams
            //   axCameraStop(session->camera);
            //   // TODO stop audio stream
            //   clientStopListening(session->client);

            //   isRunning = 0;
            // }
          } break;

          case 3: {
            int F = axWindowIsFullscreen(window);
            axWindowSetFullscreen(session->window[1], !F, 0);
            axWindowSetFullscreen(session->window[0], !F, 0);
          } break;
          }
        }
      }
    }
  }

  prevButtonState = axWindowGetKeyState(window, AX_WINDOW_KEY_MOUSE_LEFT);
}

// Render Loading Bar
void renderLoading(axRenderer renderer, int enable) {
  static float prev = 0;
  float val = sin(axClockNow());
  if (enable && val > prev) {
    float width = 1.0f;
    struct axRendererDrawInfo drawInfo = {};
    drawInfo.style = 0;
    drawInfo.corner[1] = drawInfo.corner[3] = 0.4f;
    drawInfo.size.x = width;
    drawInfo.size.y = 0.1f;
    drawInfo.center.x = (2 + width) - (2 + 4 * width) * (val + 1) / 2.0f;
    drawInfo.center.y = -0.6f;

    axRendererDraw(renderer, &drawInfo);
  }
  prev = val;
}

// do not confuse with windows update
int updateWindows(struct Session *session) {
  int exit = axWindowUpdate(
      session->window[0]); // && axWindowUpdate(session->window[1]);

  static int fullscreenButtonDown[2];

  axRendererUpdate(session->renderer[0]);
  axRendererUpdate(session->renderer[1]);
  axRendererClear(session->renderer[0], (axColor){});
  axRendererClear(session->renderer[1], (axColor){});

  // Exit on [ESC]
  if (axWindowGetKeyState(session->window[0], AX_WINDOW_KEY_ESCAPE) ||
      axWindowGetKeyState(session->window[1], AX_WINDOW_KEY_ESCAPE))
    axWindowExit(session->window[0]), axWindowExit(session->window[1]);

  // Make Windows Fullscreen
  if (axWindowGetKeyState(session->window[0], AX_WINDOW_KEY_F)) {
    if (!fullscreenButtonDown[0]) {
      fullscreenButtonDown[0] = 1;
      int F = axWindowIsFullscreen(session->window[0]);
      axWindowSetFullscreen(session->window[1], !F, 0);
      axWindowSetFullscreen(session->window[0], !F, 0);
    }
  } else
    fullscreenButtonDown[0] = 0;

  return exit;
}