#include "log.hpp"
#include <client.hpp>

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

Client::Client(int port, const char *address) {
  struct sockaddr_in serv_addr;
  struct hostent *server;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    Log::error("Socket %i: Failed to open socket", port);
    return;
  }

  if ((server = gethostbyname(address)) == NULL) {
    Log::error("Socket %i: No such host", port);
    return;
  }

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(port);
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    Log::error("Socket %i: Failed to connect", port);
    return; // TODO autoconnect
  }
}

Client::~Client() { close(sockfd); }

void Client::send(std::string msg) {
  int n = write(sockfd, msg.data(), msg.size());
  if (n != msg.size())
    Log::warning("Socket: Failed to send message '%s'", msg.c_str());
}