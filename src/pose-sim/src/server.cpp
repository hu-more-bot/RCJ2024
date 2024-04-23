#include <server.hpp>

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdexcept>

#include <cstring>

#define TIMEOUT -1 // none //2 * 60 // 2 minutes

struct Sock {
  int id;
  int latest; // latest update from client

  pthread_t service, timeouter; // threads

  Server *head;
};

void *Server::service(void *arg) {
  Sock *sock = (Sock *)arg;

  int len;
  char buffer[256];

  // printf("started service\n");

  while (sock->id >= 0) {
    bzero(buffer, 256);
    if ((len = read(sock->id, buffer, sizeof(buffer))) < 0)
      continue;

    sock->latest = time(0);

    len--;
    buffer[len] = 0;

    // handle exit
    if (!strncasecmp(buffer, "exit", 4)) {
      int id = sock->id;
      sock->id = -1;
      close(id);
      // free(sock);

      break;
    }

    sock->head->m_callback(buffer, len);

    sleep(1);
  }

  // printf("connection closed\n");

  return NULL;
}

void *Server::timeouter(void *arg) {
  Sock *sock = (Sock *)arg;

  do {
    sleep(1);

    if (sock->latest + TIMEOUT < time(0)) {
      fprintf(stderr, "timeouted connection\n");
      int id = sock->id;
      sock->id = -1;
      close(id);
      // free(sock);
    }
  } while (sock->id >= 0);

  return NULL;
}

void *Server::accepter(void *arg) {
  // int *sockfd = (int *)arg;
  auto sd = (Server *)arg;

  // Open Socket
  if ((sd->m_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    throw std::runtime_error("failed to open socket");

  // Start Server
  struct sockaddr_in serv_addr {};
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(sd->m_port);
  if (bind(sd->m_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    throw std::runtime_error("failed to bind port");

  listen(sd->m_sockfd, 5);

  while (true) {
    // Accept Connection
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int new_socket = accept(sd->m_sockfd, (struct sockaddr *)&client, &len);

    if (new_socket < 0) {
      fprintf(stderr, "Failed to accept connection\n");
      continue;
    }

    printf("new connection\n");

    // if (sock->id == -1) {
    // Launch threads
    Sock *sock = (Sock *)malloc(sizeof(Sock)); // TODO fix memory leak
    sock->id = new_socket;
    sock->latest = time(0);
    sock->head = sd;
    pthread_create(&sock->service, NULL, service, sock);
    if (TIMEOUT > 0)
      pthread_create(&sock->timeouter, NULL, timeouter, sock);

    // }
  }

  return NULL;
}

Server::Server(const std::function<void(char *, int)> &callback, int port)
    : m_callback(callback), m_port(port) {
  // socklen_t clilen = sizeof(cli_addr);
  // int newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
  // if (newsockfd < 0)
  //   fprintf(stderr, "ERROR on accepting connection\n");

  // char buffer[256];
  // bzero(buffer, 256);
  // int n = read(newsockfd, buffer, 255);
  // if (n < 0)
  //   error("ERROR reading from socket");
  // printf("Here is the message: %s\n", buffer);
  // n = write(newsockfd, "I got your message", 18);
  // if (n < 0)
  //   error("ERROR writing to socket");
  // close(newsockfd);
  // close(sockfd);
  // return 0;

  // Launch threads
  pthread_create(&m_accept, NULL, accepter, this);
}

Server::~Server() {
  pthread_cancel(m_accept);
  close(m_sockfd);
}