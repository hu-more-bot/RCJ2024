#include <server.h>

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <Artifex/log.h>

#define TAG "server"

// Connection Timeout (in seconds)
#define TIMEOUT 10

struct Server {
  int sockfd;

  int running, connected;

  serverCallback cb;
  void *user_ptr;

  pthread_t accepter, listener, timeouter;

  // std::unordered_map<int, time_t> clients;
};

int serverIsOK(server_t server) {
  if (!server || server->sockfd == 0)
    return 0;
  return 1;
}

int serverCreate(server_t *server, int port) {
  if (!server)
    return 1;

  server_t sock = *server = malloc(sizeof(struct Server));
  if (!sock) {
    ax_error(TAG, "failed to allocate memory");
    return 1;
  }

  sock->sockfd = 0;
  sock->running = sock->connected = 0;
  sock->accepter = sock->listener = sock->timeouter = 0;
  sock->user_ptr = NULL;

  // Open Socket
  if ((sock->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    free(*server);
    *server = NULL;

    ax_error(TAG, "failed to open socket");
    return 1;
  }

  // Start Server
  struct sockaddr_in serv_addr = {};
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if (bind(sock->sockfd, &serv_addr, sizeof(serv_addr)) < 0) {
    close(sock->sockfd);
    free(*server);
    *server = NULL;

    ax_error(TAG, "failed to bind port");
    return 1;
  }

  ax_verbose(TAG, "initialized");
  return 0;
}

void serverDestroy(server_t *server) {
  if (!server || !*server)
    return;

  serverStop(*server);

  close((*server)->sockfd);
  free(*server);
  *server = NULL;

  ax_verbose(TAG, "destroyed");
}

void *server__cb_accepter(void *user_ptr);
void *server__cb_listener(void *user_ptr);
// void *server__cb_timeouter(void *user_ptr);

int serverStart(server_t server, serverCallback cb, void *user_ptr) {
  if (!serverIsOK(server) || server->running)
    return 1;

  server->cb = cb;
  server->user_ptr = user_ptr;
  server->running = 1;

  // Start Threads
  pthread_create(&server->accepter, NULL, server__cb_accepter, server);
  pthread_create(&server->listener, NULL, server__cb_listener, server);
  // pthread_create(&server->timeouter, NULL, server__cb_timeouter, server);

  ax_verbose(TAG, "started");
  return 0;
}

void serverStop(server_t server) {
  if (!serverIsOK(server) || !server->running)
    return;

  server->running = 0;

  pthread_join(server->accepter, NULL);
  pthread_join(server->listener, NULL);
  // pthread_join(server->timeouter, NULL);

  // Close Connections
  // clients.clear();
}

int serverSend(server_t server, int client, char *message, unsigned long len) {
  if (!serverIsOK(server))
    return 1;

  int n = write(client, message, len);
  if (n != strlen(message)) {
    ax_warning(TAG, "failed to send message");
    return 1;
  }

  ax_verbose(TAG, "sent message '%s'", message);
  return 0;
}

// callbacks

void *server__cb_accepter(void *user_ptr) {
  server_t server = user_ptr;

  listen(server->sockfd, 5);

  while (server->running) {
    // Accept Connection
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int new_socket = accept(server->sockfd, (struct sockaddr *)&client, &len);

    if (new_socket < 0) {
      ax_warning(TAG, "failed to accept connection");
      continue;
    }

    ax_verbose(TAG, "new connection (%i)", new_socket);

    // clients[new_socket] = time(0);

    struct serverEvent event;
    event.type = CONNECTION;
    event.server = server;
    event.client = new_socket;
    event.data = NULL, event.len = 0;

    server->cb(&event, server->user_ptr);
  }
}

void *server__cb_listener(void *user_ptr) {
  server_t server = user_ptr;

  int len;
  char buffer[256];

  while (server->running) {
    // for (auto &[sockfd, upd] : clients) {
    //   bzero(buffer, 256);
    //   if ((len = read(sockfd, buffer, sizeof(buffer))) < 0) {
    //     sleep(1);
    //     continue;
    //   }

    //   upd = time(0);

    //   len--;
    //   buffer[len] = 0;

    //   // handle exit
    //   if (!strncasecmp(buffer, "exit", 4)) {
    //     close(sockfd);
    //     clients.erase(sockfd);
    //     continue;
    //   }

    //   struct serverEvent event;
    //   event.type = MESSAGE;
    //   event.sockfd = sockfd;
    //   event.data = buffer;
    //   event.len = len;

    //   callback(*this, event);
    // }
  }
}

// void *server__cb_timeouter(void *user_ptr) {
//   server_t server = user_ptr;

//   // TODO maybe move to listener thread
//   while (running && clients.size() > 0) {
//     for (auto c : clients) {
//       if (c.time + TIMEOUT < time(0)) {
//         printf("connection timed out\n");
//         close(c.sockfd);
//         clients.erase(c);
//       }
//     }

//     sleep(1);
//   }
// }