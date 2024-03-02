#include <renderer.hpp>
#include <sd-server.hpp>
#include <sd.hpp>

#include <thread>
#include <string.h>

static void onMessage(char *message)
{
  printf("todo\n");
}

int main()
{
  printf("Starting Server...  ");
  SDServer sdServer(onMessage, 8000);
  printf("Done\n");

  printf("Initializing Renderer...  ");
  Renderer renderer("asd");
  printf("Done\n");

  printf("Loading Stable Diffusion...  ");
  // SD sd("../models/sd-v1-4.ckpt");
  // sd.config.width = renderer.width;
  // sd.config.height = renderer.height;
  printf("Done\n");

  // std::thread render(SD::txt2img, "../models/...", "prompt");

  while (renderer.update())
  {
    if (renderer.key("esc"))
      renderer.exit();

    // if (render.joinable())
    renderer.draw();

    // sdServer.update();
  }

  // while (!render.joinable())
  //   ;

  // render.detach();

  return 0;
}