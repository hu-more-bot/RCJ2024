#pragma once

#include <string>

// Simple socket() client
class Client {
public:
  Client(int port, const char *address = "localhost");
  ~Client();

  void send(std::string msg);

private:
  int sockfd;
};