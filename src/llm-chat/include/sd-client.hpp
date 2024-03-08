#pragma once

#include <string>

class SDClient {
public:
  SDClient(int port = 8000, const char *address = "localhost");
  ~SDClient();

  void send(std::string msg);

private:
  int sockfd;
};