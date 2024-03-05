#include <sd-client.hpp>

#include <stdexcept>

#define PROTOCOL_NAME "sd"

static void *service(void *arg) {
  while (true)
    lws_service((struct lws_context *)arg, 0);

  return NULL;
}

SDClient::SDClient(int port, const char *address) {
  // Create Context
  static struct lws_protocols protocols[] = {
      {PROTOCOL_NAME, callback, sizeof(void *), 0, 0, this}, {}};

  struct lws_context_creation_info info = {};

  info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
  info.protocols = protocols;

  if (!(context = lws_create_context(&info)))
    throw std::runtime_error("SDClient: failed to init lws");

  // Connect
  struct lws_client_connect_info i = {};

  i.protocol = i.local_protocol_name = PROTOCOL_NAME;
  i.address = i.host = i.origin = address;
  i.context = context;
  i.port = port;

  if (!(wsi = lws_client_connect_via_info(&i)))
    throw std::runtime_error("SDClient: failed to init wsi");

  // Launch Service
  pthread_create(&thread, NULL, service, context);
}

SDClient::~SDClient() { lws_context_destroy(context); }

void SDClient::send(std::string msg) {
  if (wsi) {
    const size_t len = msg.size();
    char buf[LWS_PRE + len];

    memcpy(&buf[LWS_PRE], msg.data(), len);
    lws_write(wsi, (unsigned char *)&buf[LWS_PRE], len, LWS_WRITE_TEXT);
  } else {
    printf("SDClient: failed to connect\n");
  }
}

int SDClient::callback(struct lws *wsi, enum lws_callback_reasons reason,
                       void *user, void *in, size_t len) {
  SDClient *self = (SDClient *)user;

  switch (reason) {
  case LWS_CALLBACK_CLIENT_ESTABLISHED:
    printf("SDClient: connection established\n");
    break;

  case LWS_CALLBACK_CLIENT_CLOSED:
    printf("SDClient: connection closed\n");
    break;

  default:
    break;
  }

  return 0;
}