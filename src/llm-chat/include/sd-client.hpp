#pragma once

#include <libwebsockets.h>

#include <atomic>
#include <pthread.h>

#include <string>

class SDClient {
public:
  SDClient(int port = 8000, const char *address = "localhost");
  ~SDClient();

  void send(std::string msg);

private:
  struct lws_context *context{};
  struct lws *wsi{};

  pthread_t thread;

  static int callback(struct lws *wsi, enum lws_callback_reasons reason,
                      void *user, void *in, size_t len);
};