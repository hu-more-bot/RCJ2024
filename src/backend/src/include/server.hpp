#pragma once

#include <functional>
#include <set>
#include <string>
#include <thread>

// Simple Linux TCP Server
class Server {
public:
  struct Event {
    // Event Types
    enum {
      CONNECTION,
      DISCONNECTION,
      TIMEOUT,

      MESSAGE
    } type;

    // Related Sock
    int sockfd;

    char *data;
    size_t len;
  };

  Server(int port,
         std::function<void(Server &server, const Event &event)> callback);
  ~Server();

  void send(int sockfd, void *data, unsigned long size);

private:
  int sockfd;

  bool running;
  std::thread accepter, timeouter;

  struct Client {
    time_t activity;
    int fd;

    std::thread *listener;
  };

  std::unordered_map<int, Client> clients;
};