#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Simple Linux TCP Client
typedef struct Client *client_t;

struct clientEvent {
  // Event Types
  enum {
    CONNECTION,
    DISCONNECTION,
    TIMEOUT,

    MESSAGE
  } type;

  client_t client;

  const char *data;
  unsigned long len;
};

typedef void (*clientCallback)(const struct clientEvent *, void *);

// Check if client is healthy
int clientIsOK(client_t client);

// Create Client
int clientCreate(client_t *client);

// Destroy Client
void clientDestroy(client_t *client);

// Open Connection
int clientOpen(client_t client, const char *address, int port);

// Close Connection
void clientClose(client_t client);

// Send Message
int clientSend(client_t client, const char *message, unsigned long len);

// Start Listening
int clientStartListening(client_t client, clientCallback cb, void *user_ptr);

// Stop Listening
int clientStopListening(client_t client);

// Is Connected
int clientIsConnected(client_t client);

#ifdef __cplusplus
}
#endif