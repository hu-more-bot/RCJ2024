#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Simple Linux TCP Server
typedef struct Server *server_t;

struct serverEvent {
  // Event Types
  enum {
    CONNECTION,
    DISCONNECTION,
    TIMEOUT,

    MESSAGE
  } type;

  server_t server;
  unsigned int client;

  const char *data;
  unsigned long len;
};

typedef void (*serverCallback)(const struct serverEvent *, void *);

// Check if client is healthy
int serverIsOK(server_t server);

// Create Server
int serverCreate(server_t *server, int port);

// Destroy Client
void serverDestroy(server_t *server);

// Start Listening
int serverStart(server_t server, serverCallback cb, void *user_ptr);

// Stop Listening
void serverStop(server_t server);

// Send Message
int serverSend(server_t server, int client, char *message, unsigned long len);

#ifdef __cplusplus
}
#endif