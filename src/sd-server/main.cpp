#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <renderer.hpp>
#include <sd-server.hpp>
#include <sd.hpp>

#include <SDL2/SDL.h>

#include <cstring>
#include <ctime>

static void callback(char *message, int len, void *user);

// Return time in ms
float time() {
  struct timespec res {};
  clock_gettime(CLOCK_MONOTONIC, &res);
  return (float)(1000.0f * res.tv_sec + (float)res.tv_nsec / 1e6) / 1000.0f;
}

int main() {
  printf("Initializing Renderer...\n");
  // Renderer renderer("RCJ2024");
  // The window we'll be rendering to
  SDL_Window *window = NULL;

  // The surface contained by the window
  SDL_Surface *screenSurface = NULL;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
  }

  window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, 768, 1366,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
  if (window == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    exit(1);
  }

  printf("Loading Stable Diffusion...\n");
  // SD sd("../models/sd-v1-4.ckpt");
  // sd.config.width = 768;   // renderer.width;
  // sd.config.height = 1366; // renderer.height;

  printf("Starting Server...\n");
  // SDServer sdServer(callback, &sd);

  // std::vector<float> verts = {
  //     0.0, -1.5, 0, 0, //
  //     0.3, 0.0,  0, 0, //
  //     0.0, 0.3,  0, 0, //
  //     0.3, 0.0,  0, 0  //
  // };
  SDL_Surface *window_surface = SDL_GetWindowSurface(window);

  // SDL_Surface image;
  // image.w = 512;
  // image.h = 512;
  // image.pixels = {255, 255, 0};
  // SDL_CreateRGBSurfaceFrom(data, width, height, bytesPerPixel * 8, pitch,
  // Rmask,
  //  Gmask, Bmask, Amask);
  unsigned char data[] = {255, 140, 0};
  SDL_Surface *image = SDL_CreateRGBSurfaceFrom(data, 1, 1, 1, 0, 0, 0, 0, 0);

  bool keep_window_open = true;
  while (keep_window_open) {
    bool keep_window_open = true;
    while (keep_window_open) {
      SDL_Event e;
      while (SDL_PollEvent(&e) > 0) {
        switch (e.type) {
        case SDL_QUIT:
          keep_window_open = false;
          break;
        }

        SDL_BlitSurface(image, NULL, window_surface, NULL);
        SDL_UpdateWindowSurface(window);
      }
    }
  }

  // // float past, now = time();
  // while (renderer.update()) {
  //   // past = now;
  //   // now = time();
  //   // float deltaTime = now - past;

  //   if (renderer.key("esc"))
  //     renderer.exit();

  //   renderer.clear();

  //   // if (render.joinable())
  //   // if (sd.show)
  //   renderer.draw(0, 0);

  //   SDL_UpdateWindowSurface(window);

  //   // sdServer.update();
  // }

  // Destroy window
  SDL_DestroyWindow(window);

  // Quit SDL subsystems
  SDL_Quit();

  // while (!render.joinable())
  //   ;

  // render.detach();

  return 0;
}

static void callback(char *message, int len, void *user) {
  auto sd = (SD *)user;

  if (!strncasecmp(message, "PAINT", 5))
    goto paint;
  // else if (!strncasecmp(message, "SHOW", 4))
  //   goto show;
  return;

paint:
  printf("generating image\n");
  sd->config.prompt = message + sizeof("PAINT: ");
  sd->config.seed = time(0);
  sd->generate();

  return;
}