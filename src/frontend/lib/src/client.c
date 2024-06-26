#include <client.h>

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <Artifex/log.h>

#define TAG "client"

// Simple Linux TCP Client
struct Client {
  int sockfd;

  int running, connected;

  clientCallback cb;
  void *user_ptr;

  pthread_t listener;
};

int clientIsOK(client_t client) {
  if (!client || client->sockfd == 0)
    return 0;
  return 1;
}

int clientCreate(client_t *client) {
  if (!client)
    return 1;

  client_t sock = *client = malloc(sizeof(struct Client));
  if (!sock) {
    ax_error(TAG, "failed to allocate memory");
    return 1;
  }

  sock->sockfd = 0;
  sock->running = sock->connected = 0;
  sock->listener = 0;
  sock->user_ptr = NULL;

  // Open Socket
  if ((sock->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    free(sock);
    sock = NULL;

    ax_error(TAG, "failed to open socket");
    return 1;
  }

  ax_verbose(TAG, "initialized");
  return 0;
}

void clientDestroy(client_t *client) {
  if (!client || !*client)
    return;

  clientStopListening(*client);

  close((*client)->sockfd);
  free(*client);
  *client = NULL;

  ax_verbose(TAG, "destroyed");
}

int clientOpen(client_t client, const char *address, int port) {
  if (!clientIsOK(client) || client->connected)
    return 1;

  // Get Host
  struct hostent *server;
  if ((server = gethostbyname(address)) == NULL) {
    ax_error(TAG, "no such host");
    return 1; // no such host
  }

  // Connect to Server
  struct sockaddr_in serv_addr = {};
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);

  serv_addr.sin_port = htons(port);
  if (connect(client->sockfd, (struct sockaddr *)&serv_addr,
              sizeof(serv_addr)) < 0) {
    ax_error(TAG, "failed to connect");
    return 1; // failed to connect
  }
  client->connected = 1;

  ax_verbose(TAG, "connected");
  return 0;
}

void clientClose(client_t client) {
  if (!clientIsOK(client))
    return;

  clientStopListening(client);

  shutdown(client->sockfd, SHUT_RDWR);
  client->connected = 0;

  ax_verbose(TAG, "closed");
}

int clientSend(client_t client, const char *message) {
  int n = write(client->sockfd, message, strlen(message));
  if (n != strlen(message)) {
    ax_error(TAG, "failed to send message");
    return 1;
  }

  ax_verbose(TAG, "sent message '%s'", message);
  return 0;
}

int clientIsConnected(client_t client) {
  if (!clientIsOK(client))
    return 0;

  return client->connected;
}

void *client___callback(void *user) {
  client_t client = user;

  pthread_detach(pthread_self());

  unsigned long len;
  char buffer[256];

  while (client->running) {
    bzero(buffer, 256);
    if ((len = read(client->sockfd, buffer, sizeof(buffer))) < 0) {
      sleep(1);
      continue;
    }

    len--;
    buffer[len] = 0;

    struct clientEvent event;
    event.type = MESSAGE;
    event.data = buffer;
    event.len = len;

    client->cb(&event, client->user_ptr);
  }

  return NULL;
}

int clientStartListening(client_t client, clientCallback cb, void *user_ptr) {
  if (!clientIsOK(client) || !client->connected || client->running)
    return 1;

  client->cb = cb;
  client->user_ptr = user_ptr;
  client->running = 1;
  pthread_create(&client->listener, NULL, client___callback, client);

  ax_verbose(TAG, "listening started");

  return 0;
}

int clientStopListening(client_t client) {
  if (!clientIsOK(client) || !client->listener)
    return 1;

  client->running = 0;
  pthread_join(client->listener, NULL);

  ax_verbose(TAG, "listening stopped");

  return 0;
}