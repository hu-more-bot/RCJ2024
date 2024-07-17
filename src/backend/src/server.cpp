#include <server.hpp>

#include <stdexcept>

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Number of ports to try after specified one
#define PORT_ATTEMPTS 10

// Connection Timeout (in seconds)
#define TIMEOUT 10

Server::Server(
    int port,
    std::function<void(Server &server, const Event &event)> callback) {
  // Open Socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    throw std::runtime_error("failed to open socket");

  // Start Server
  struct sockaddr_in serv_addr = {};
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  bool success = false;
  for (int i = 0; i < PORT_ATTEMPTS; i++) {
    serv_addr.sin_port = htons(port + i);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      printf("failed to bind port %i\n", port + i);
    else {
      success = true;
      break;
    }
  }

  if (!success)
    throw std::runtime_error("failed to bind port");

  // Start Accepter Thread
  accepter = (std::thread)[&] {
    listen(sockfd, 5);

    while (running) {
      // Accept Connection
      struct sockaddr_in client;
      socklen_t len = sizeof(client);
      int new_socket = accept(sockfd, (struct sockaddr *)&client, &len);

      if (new_socket < 0) {
        fprintf(stderr, "Failed to accept connection\n");
        continue;
      }

      printf("new connection\n");

      // Listener
      clients[new_socket] = {
          time(0), new_socket, new std::thread([&] {
            int len;
            char buffer[256];

            while (running && clients.size() > 0) {
              for (auto &[sockfd, cli] : clients) {
                bzero(buffer, 256);
                if ((len = read(sockfd, buffer, sizeof(buffer))) < 0) {
                  sleep(1);
                  continue;
                }

                cli.activity = time(0);

                len--;
                buffer[len] = 0;

                // handle exit
                if (!strncasecmp(buffer, "exit", 4)) {
                  close(sockfd);
                  clients.erase(sockfd);
                  continue;
                }
                printf("asd\n");

                Event event;
                event.type = Event::MESSAGE;
                event.sockfd = sockfd;
                event.data = buffer;
                event.len = len;

                callback(*this, event);
              }
            }
          })};

      Event event;
      event.type = Event::CONNECTION;
      event.sockfd = new_socket;

      callback(*this, event);
    }
  };

  // timeouter = new (std::thread)[&] {
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
  // };
}

Server::~Server() {
  running = false;
  accepter.join();

  // Close Connections
  clients.clear();

  timeouter.join();

  close(sockfd);
}

void Server::send(int sockfd, void *data, unsigned long size) {
  if (sockfd < 0) {
    // for (auto [s, _] : clients)
    // send(s, data, size);
  } else {
    int n = write(sockfd, data, size);
    if (n != size)
      fprintf(stderr, "%s: failed to send message\n", __func__);
  }
}