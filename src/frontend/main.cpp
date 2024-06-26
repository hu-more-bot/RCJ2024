// #include <client.hpp>
// #include <stt.hpp>
// #include <tts.hpp>

#include <Artifex/camera.h>
#include <Artifex/log.h>
#include <Artifex/renderer.h>

#include <client.h>
#include <yolo.h>

// #include <tts.h>

#include <math.h>
#include <stdlib.h>

static float person = 0;

#include "image.h"
#include "render.h"

#include <stt.h>

int main() {
  // STT stt("../models/ggml-tiny.en-q5_1.bin");

  // while (1) {
  //   std::string txt = stt.listen();
  //   printf("%s\n", txt.c_str());
  // }

  // exit(0);
  // Create Camera
  axCamera camera;
  axCameraCreate(&camera, 640, 480, 30);

  // Create YOLO
  yolo_t yolo;
  yoloCreate(&yolo, "../models/yolov7-tiny.onnx");

  // tts_t tts;
  // ttsCreate(&tts, "../models/piper/ryan.onnx");

  stt_t stt;
  sttCreate(&stt, "../models/ggml-tiny.en-q5_1.bin");

  // Create Client
  client_t client;
  clientCreate(&client);

  // Create Windows
  axWindow wnd_face, wnd_chest;
  axWindowCreate(&wnd_face, "Face", 720, 480);
  // axWindowCreate(&wnd_chest, "Chest", 720, 480);

  axRenderer rnd_face, rnd_chest;
  axRendererCreate(&rnd_face, wnd_face);
  // axRendererCreate(&rnd_chest, wnd_chest);

  // ---- Load Textures

  // UI Textures (close, link, start, fullscreen, unlink, stop, fullscreen exit)
  unsigned int texUI[7] = {
      loadTexture(rnd_face, "../res/ui/close.png"),
      loadTexture(rnd_face, "../res/ui/link.png"),
      loadTexture(rnd_face, "../res/ui/start.png"),
      loadTexture(rnd_face, "../res/ui/fullscreen.png"),
      loadTexture(rnd_face, "../res/ui/link_off.png"),
      loadTexture(rnd_face, "../res/ui/stop.png"),
      loadTexture(rnd_face, "../res/ui/fullscreen_exit.png"),
  };

  // Face Textures (eye x2, eyebrow x2, mouth)
  unsigned int texFace[5] = {
      loadTexture(rnd_face, "../res/pupil.png"),
      loadTexture(rnd_face, "../res/pupil.png"),
      loadTexture(rnd_face, "../res/eyebrow_fancy_left.png"),
      loadTexture(rnd_face, "../res/eyebrow_fancy_right.png"),
      loadTexture(rnd_face, "../res/mouth_monster.png"),
  };

  ax_verbose("main", "initialization done");

  while (axWindowUpdate(wnd_face)) { // && axWindowUpdate(wnd_chest)) {
    /* Update Windows */ {
      static int fullscreenButtonDown[2];

      axRendererUpdate(rnd_face);
      // axRendererUpdate(rnd_chest);
      axRendererClear(rnd_face, (axColor){1, 1, 1});
      // axRendererClear(rnd_chest, (axColor){});

      // Exit on [ESC]
      if (axWindowGetKeyState(wnd_face, AX_WINDOW_KEY_ESCAPE))
        //  || axWindowGetKeyState(wnd_chest, AX_WINDOW_KEY_ESCAPE))
        axWindowExit(wnd_face); //, axWindowExit(wnd_chest);

      // Make Windows Fullscreen
      if (axWindowGetKeyState(wnd_face, AX_WINDOW_KEY_F)) {
        if (!fullscreenButtonDown[0]) {
          fullscreenButtonDown[0] = 1;
          axWindowSetFullscreen(wnd_face, !axWindowIsFullscreen(wnd_face), 0);
        }
      } else
        fullscreenButtonDown[0] = 0;

      // if (axWindowGetKeyState(wnd_chest, AX_WINDOW_KEY_F)) {
      //   if (!fullscreenButtonDown[1]) {
      //     fullscreenButtonDown[1] = 1;
      //     axWindowSetFullscreen(wnd_chest, !axWindowIsFullscreen(wnd_chest),
      //     0);
      //   }
      // } else
      //   fullscreenButtonDown[1] = 0;
    }

    renderFace(rnd_face, texFace, person, 1.1f);
    renderUI(wnd_face, rnd_face, texUI, client, camera, yolo);
    // renderLoading(rnd_face, 1);
  }

  ax_verbose("main", "stopped, cleaning up");

  // Clean Up Windows
  axRendererDestroy(&rnd_face);
  // axRendererDestroy(&rnd_chest);

  axWindowDestroy(&wnd_face);
  // axWindowDestroy(&wnd_chest);

  // Clean Up Client
  clientDestroy(&client);

  // Clean Up YOLO & Camera
  yoloDestroy(&yolo);
  axCameraDestroy(&camera);

  ax_verbose("main", "cleanup done, exiting");

  return 0;
}