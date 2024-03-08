#include <renderer.hpp>
#include <sd-server.hpp>
#include <sd.hpp>

#include <cstring>
#include <ctime>

static void callback(char *message, int len, void *user);

// Return time in ms
float time() {
  struct timespec res {};
  clock_gettime(CLOCK_MONOTONIC, &res);
  return (float)(1000.0f * (float)res.tv_sec + (float)res.tv_nsec / 1e6) /
         1000.0f;
}

int main() {
  printf("Initializing Renderer...\n");
  Renderer renderer("RCJ2024");

  printf("Loading Stable Diffusion...\n");
  SD sd("../models/sd-v1-4.ckpt");
  sd.config.width = renderer.width;
  sd.config.height = renderer.height;

  printf("Starting Server...\n");
  SDServer sdServer(callback, &sd);

  // std::vector<float> verts = {
  //     0.0, -1.5, 0, 0, //
  //     0.3, 0.0,  0, 0, //
  //     0.0, 0.3,  0, 0, //
  //     0.3, 0.0,  0, 0  //
  // };

  // std::vector<GLuint> inds = {0, 1, 3, 1, 2, 3};

  // renderer.mesh(verts, inds);

  // std::thread render(SD::txt2img, "../models/...", "prompt");

  float past, now = time();
  while (renderer.update()) {
    past = now;
    now = time();
    float deltaTime = now - past;

    if (renderer.key("esc"))
      renderer.exit();

    renderer.clear();

    // if (render.joinable())
    // if (sd.show)
    // renderer.draw(1);

    // sdServer.update();
  }

  // while (!render.joinable())
  //   ;

  // render.detach();

  return 0;
}

static void callback(char *message, int len, void *user) {
  auto sd = (SD *)user;

  printf("new message: '%s'\n", message);
  if (!strcasecmp(message, "PAINT"))
    goto paint;
  else if (!strcasecmp(message, "SHOW"))
    goto show;
  return;

paint:
  printf("generating image\n");
  sd->show = false;
  sd->config.prompt = message;
  sd->generate();
  // TODO properly generate image
  return;

show:
  printf("showing image\n");

  sd->show = true;
  // TODO show image
  return;
}