#include <session.h>

#include "Artifex/renderer.h"
#include "callbacks.h"
#include "client.h"
#include "image.h"
#include "render.h"

int main() {
  struct Session session = {};

  // Initialize
  axCameraCreate(&session.camera, 640, 480, 30);
  yoloCreate(
      &session.yolo,
      "../../../../models/yolo/yolov7.onnx", // getenv("MODEL_YOLOv7"),
      "../../../../models/yolo/yolov7.txt"); // getenv("MODEL_YOLOv7_PARAMS"));

  clientCreate(&session.client, "localhost", 8000);

  axWindowCreate(&session.window[0], "Face", 720, 480);
  axWindowCreate(&session.window[1], "Chest", 720, 480);

  axRendererCreate(&session.renderer[0], session.window[0]);
  axRendererCreate(&session.renderer[1], session.window[1]);

  // ---- Load Textures

  // UI Textures (close, link, start, fullscreen, unlink, stop, fullscreen exit)
  unsigned int texUI[7] = {
      loadTexture(session.renderer[0], "../res/close.png"),
      loadTexture(session.renderer[0], "../res/link.png"),
      loadTexture(session.renderer[0], "../res/start.png"),
      loadTexture(session.renderer[0], "../res/fullscreen.png"),
      loadTexture(session.renderer[0], "../res/link_off.png"),
      loadTexture(session.renderer[0], "../res/stop.png"),
      loadTexture(session.renderer[0], "../res/fullscreen_exit.png"),
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

    // Update Emote Animation

    // Render Face UI
    renderFace(session.renderer[0], session.person, 0);
    renderUI(session.window[0], session.renderer[0], texUI, &session);
    // renderLoading(rnd_face, 1);

    // Render Chest UI
    struct axRendererDrawInfo drawInfo;
    drawInfo.size = (axVector){0.9, 1.6};
    drawInfo.style = image == 0 ? 0 : 2;
    drawInfo.texture = image;
    drawInfo.corner[0] = drawInfo.corner[1] = drawInfo.corner[2] =
        drawInfo.corner[3] = 0.3f;
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

  // yoloDestroy(&session.yolo);
  axCameraDestroy(&session.camera);

  ax_verbose("main", "cleanup done, exiting");

  return 0;
}