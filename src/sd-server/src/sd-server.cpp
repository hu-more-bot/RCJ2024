#include <sd-server.hpp>

#include <stdexcept>

#define TIMEOUT -1 // none //2 * 60 // 2 minutes
#define PORT 8000

struct Sock {
  int id;
  int latest; // latest update from client

  pthread_t service, timeouter; // threads

  SDServer *head;
};

static void *service(void *arg) {
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

    sock->head->callback(buffer, len, sock->head->user);

    sleep(1);
  }

  // printf("connection closed\n");

  return NULL;
}

static void *timeouter(void *arg) {
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

static void *accepter(void *arg) {
  // int *sockfd = (int *)arg;
  auto sd = (SDServer *)arg;

  // Open Socket
  if ((sd->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    throw std::runtime_error("failed to open socket");

  // Start Server
  struct sockaddr_in serv_addr {};
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT);
  if (bind(sd->sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    throw std::runtime_error("failed to bind port");

  listen(sd->sockfd, 5);

  while (true) {
    // Accept Connection
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int new_socket = accept(sd->sockfd, (struct sockaddr *)&client, &len);

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

SDServer::SDServer(messageCallback callback, void *user, int port)
    : callback(callback), user(user) {
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
  pthread_create(&accept, NULL, accepter, this);
}

SDServer::~SDServer() {
  pthread_cancel(accept);
  close(sockfd);
}