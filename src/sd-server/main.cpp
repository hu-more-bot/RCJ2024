#include <renderer.hpp>

#include <libwebsockets.h>

#include <thread>
#include <string.h>

#define SD_SERVER_PORT 8000
#define SD_PROTOCOL "sd-server"

static int callback(struct lws *wsi, enum lws_callback_reasons reason,
                    void *user, void *in, size_t len);

int main()
{
  // Renderer renderer("asd");
  // std::thread render(SD::txt2img, "../models/...", "prompt");

  struct lws_context *context;

  /* Start sd-server */ {
    struct lws_protocols protocols[] = {{SD_PROTOCOL, callback}, {}};

    struct lws_context_creation_info info = {};

    info.port = SD_SERVER_PORT;
    info.protocols = protocols;

    context = lws_create_context(&info);

    if (!context)
    {
      lwsl_err("lws init failed\n");
      return 1;
    }
  }

  while (true)
  {
    lws_service(context, 250);
  }

  // while (renderer.Window::update())
  // {
  //   if (renderer.key("esc"))
  //     renderer.exit();

  //   // if (render.joinable())
  //   renderer.draw();

  // }

  lws_context_destroy(context);

  // while (!render.joinable())
  //   ;

  // render.detach();

  return 0;
}

static int callback(struct lws *wsi, enum lws_callback_reasons reason,
                    void *user, void *in, size_t len)
{
  switch (reason)
  {
  case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
    printf("New Connection\n");
    break;

  case LWS_CALLBACK_RECEIVE:
    printf("Received data: %s\n", (char *)in);
    // TODO: launch sd generation
    break;

  default:
    break;
  }

  return 0;
}