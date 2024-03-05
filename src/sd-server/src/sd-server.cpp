#include <sd-server.hpp>

#include <stdexcept>

#define PROTOCOL_NAME "sd"

static void *service(void *arg) {
  while (true)
    lws_service((struct lws_context *)arg, 0);

  return NULL;
}

SDServer::SDServer(messageCallback onMessage, int port) {
  struct lws_protocols protocols[] = {
      {PROTOCOL_NAME, callback, sizeof(void *), 0, 0, this}, {}};

  struct lws_context_creation_info info = {};

  info.port = port;
  info.protocols = protocols;

  context = lws_create_context(&info);

  if (!context)
    throw std::runtime_error("SDServer: failed to init lws");

  pthread_create(&thread, NULL, service, context);
}

SDServer::~SDServer() { lws_context_destroy(context); }

int SDServer::callback(struct lws *wsi, enum lws_callback_reasons reason,
                       void *user, void *in, size_t len) {
  SDServer *self = (SDServer *)user;

  switch (reason) {
  case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
    printf("SDServer: new connection\n");
    break;

  case LWS_CALLBACK_RECEIVE:
    printf("SDServer: received data: %.*s\n", len, (char *)in);
    // (*onMessage)((char *)in);
    break;

  default:
    break;
  }

  return 0;
}