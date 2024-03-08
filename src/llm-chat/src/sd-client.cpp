#include <sd-client.hpp>

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

SDClient::SDClient(int port, const char *address) {
  struct sockaddr_in serv_addr;
  struct hostent *server;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "%s: failed to open socket\n", __func__);
    return;
  }

  if ((server = gethostbyname(address)) == NULL) {
    fprintf(stderr, "%s: no such host\n", __func__);
    return;
  }

  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(port);
  if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "%s: failed to connect\n", __func__);
    return; // TODO autoconnect
  }
}

SDClient::~SDClient() { close(sockfd); }

void SDClient::send(std::string msg) {
  int n = write(sockfd, msg.data(), msg.size());
  if (n != msg.size())
    fprintf(stderr, "%s: failed to send message\n", __func__);
}