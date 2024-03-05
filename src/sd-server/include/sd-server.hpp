#pragma once

#include <libwebsockets.h>

#include <atomic>
#include <pthread.h>

class SDServer {
public:
  typedef void (*messageCallback)(char *);

  SDServer(messageCallback onMessage, int port = 8000);
  ~SDServer();

private:
  struct lws_context *context;
  pthread_t thread;

  static int callback(struct lws *wsi, enum lws_callback_reasons reason,
                      void *user, void *in, size_t len);
};