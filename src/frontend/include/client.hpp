#pragma once

#include <functional>
#include <string>
#include <thread>

// Simple Linux TCP Client
class Client
{
public:
  struct Event
  {
    // Event Types
    enum
    {
      CONNECTION,
      DISCONNECTION,
      TIMEOUT,

      MESSAGE
    } type;

    // Related Sock
    int sockfd;

    std::string content;
  };

  Client(int port, const char *address, std::function<void(const Event &event)> callback);
  ~Client();

  void send(std::string msg);

private:
  int sockfd;

  bool running;
  std::thread listener;
};