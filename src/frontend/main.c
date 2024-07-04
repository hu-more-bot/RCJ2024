#include <session.h>

#include "Artifex/renderer.h"
#include "callbacks.h"
#include "image.h"
#include "render.h"

int main() {
  struct Session session;

  // Initialize
  axCameraCreate(&session.camera, 640, 480, 30);
  yoloCreate(&session.yolo, "../models/yolov7-tiny.onnx");

  clientCreate(&session.client);

  axWindowCreate(&session.window[0], "Face", 720, 480);
  axWindowCreate(&session.window[1], "Chest", 720, 480);

  axRendererCreate(&session.renderer[0], session.window[0]);
  axRendererCreate(&session.renderer[1], session.window[1]);

  // ---- Load Textures

  // UI Textures (close, link, start, fullscreen, unlink, stop, fullscreen exit)
  unsigned int texUI[7] = {
      loadTexture(session.renderer[0], "../res/ui/close.png"),
      loadTexture(session.renderer[0], "../res/ui/link.png"),
      loadTexture(session.renderer[0], "../res/ui/start.png"),
      loadTexture(session.renderer[0], "../res/ui/fullscreen.png"),
      loadTexture(session.renderer[0], "../res/ui/link_off.png"),
      loadTexture(session.renderer[0], "../res/ui/stop.png"),
      loadTexture(session.renderer[0], "../res/ui/fullscreen_exit.png"),
  };

  // Face Textures (eye x2, eyebrow x2, mouth)
  unsigned int texFace[5] = {
      loadTexture(session.renderer[0], "../res/pupil.png"),
      loadTexture(session.renderer[0], "../res/pupil.png"),
      loadTexture(session.renderer[0], "../res/eyebrow_fancy_left.png"),
      loadTexture(session.renderer[0], "../res/eyebrow_fancy_right.png"),
      loadTexture(session.renderer[0], "../res/mouth_monster.png"),
  };

  // image on the chest
  unsigned int image = 0;

  ax_verbose("main", "initialization done");

  // Start
  axCameraStart(session.camera, framecb, &session);

  int imageRefreshNeeded = 0;
  while (updateWindows(&session)) {
    // Refresh Image
    if (session.image.refresh == 1) {
      session.image.refresh = 0;

      if (image == 0) {
        axRendererLoadTexture(session.renderer[1], session.image.width,
                              session.image.height, session.image.channels,
                              session.image.data);
        break;
      }

      axRendererUpdateTexture(session.renderer[1], image, 0, 0,
                              session.image.width, session.image.height,
                              session.image.channels, session.image.data);
    }

    // Render Face UI
    renderFace(session.renderer[0], texFace, 0, 1.1f);
    renderUI(session.window[0], session.renderer[0], texUI, &session);
    // renderLoading(rnd_face, 1);

    // Render Chest UI
    struct axRendererDrawInfo drawInfo;
    drawInfo.size = (axVector){0.4, 0.6};
    drawInfo.style = image == 0 ? 0 : 2;
    drawInfo.texture = image;
    axRendererDraw(session.renderer[1], &drawInfo);
  }

  // Stop
  axCameraStop(session.camera);

  ax_verbose("main", "stopped, cleaning up");

  // Clean Up
  axRendererDestroy(&session.renderer[0]);
  axRendererDestroy(&session.renderer[1]);

  axWindowDestroy(&session.window[0]);
  axWindowDestroy(&session.window[1]);

  clientDestroy(&session.client);

  yoloDestroy(&session.yolo);
  axCameraDestroy(&session.camera);

  ax_verbose("main", "cleanup done, exiting");

  return 0;
}