#include <sd-server.hpp>
#include <sd.hpp>

#include <Artifex/core/renderer.hpp>

#include <cstring>
#include <ctime>

using namespace Artifex;

static void callback(char *message, int len, void *user);

// Return time in ms
float time() {
  struct timespec res {};
  clock_gettime(CLOCK_MONOTONIC, &res);
  return (float)(1000.0f * res.tv_sec + (float)res.tv_nsec / 1e6) / 1000.0f;
}
char *loadTXT(const char *path);

int main() {
  printf("Initializing Renderer...\n");
  Renderer renderer("RCJ2024", {480, 720});
  // renderer.fullscreen();

  /* Load Shader */ {
    FILE *f = fopen("../shader/main.glsl", "r");

    if (f) {
      uint8_t current = 0;

      // RAW Shader Code (trash, vertex, fragment, geometry)
      std::string code[4];

      char line[256];
      while (fgets(line, sizeof(line), f)) {
        char index[10], parameter[10];
        sscanf(line, "%9s %9s", index, parameter);

        if (!strcmp(index, "#shader")) {
          if (!strcmp(parameter, "vertex"))
            current = 1;
          else if (!strcmp(parameter, "fragment"))
            current = 2;
          else if (!strcmp(parameter, "geometry"))
            current = 3;
          else
            Log::warning("Main/Load", "Invalid Shader type: %s", parameter);
        } else if (!strcmp(index, "#script")) {
          Log::warning("Main/Load", "Shader Scripts are not supported YET!\n");
        } else {
          code[current] += line;
        }
      }

      fclose(f);

      renderer.load_shader(code[1].c_str(), code[2].c_str(), code[3].c_str());
    } else {
      Log::error("Main/Load", "Failed to Open Shader File");
    }
  }

  printf("Loading Stable Diffusion...\n");
  SD sd("../models/sd-v1-4.ckpt");
  sd.config.width = 512;  // renderer.width;
  sd.config.height = 512; // renderer.height;

  printf("Starting Server...\n");
  SDServer sdServer(callback, &sd);

  bool has_image = false;
  while (renderer.update()) {
    renderer.clear({});

    if (sd.show) {
      if (has_image) {
        glDeleteTextures(1, &renderer.textures[0]);
        renderer.textures.clear();
      }
      renderer.load_texture(sd.result->data,
                            {sd.result->width, sd.result->height},
                            sd.result->channel);
      has_image = true;
      sd.show = false;
    }

    renderer.draw({}, {0.7}, 0.0f, has_image ? 2 : 0, 0.2, {}, 0);
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

  // while (!render.joinable())
  //   ;

  // render.detach();

  return 0;
}

static void callback(char *message, int len, void *user) {
  auto sd = (SD *)user;

  if (!strncasecmp(message, "PAINT: ", 7))
    goto paint;
  // else if (!strncasecmp(message, "SHOW", 4))
  //   goto show;
  return;

paint:
  if (sd->show)
    return;

  printf("Generating Image; Prompt: '%s'\n", message + sizeof("PAINT: ") - 1);
  sd->config.prompt = message + sizeof("PAINT: ") - 1;
  sd->config.seed = time(0);
  sd->generate();
  sd->show = true;
}

char *loadTXT(const char *path) {
  // Open File
  FILE *f = NULL;
  if (!(f = fopen(path, "r"))) {
    printf("Failed to open file: %s\n", path);
    return NULL;
  }

  // Get File Size
  fseek(f, 0, SEEK_END);
  size_t size = ftell(f);
  fseek(f, 0, SEEK_SET);

  // Read File
  char *out = (char *)malloc(sizeof(char) * size);

  if (!fread(out, size, 1, f)) {
    printf("Failed to read file: %s\n", path);
    free(out);
    return NULL;
  }

  return out;
}