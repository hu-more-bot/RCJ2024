#include <client.hpp>

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

Client::Client(int port, const char *address, std::function<void(const Event &event)> callback)
{
  // Open Socket
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    throw std::runtime_error("failed to open socket");

  // Get Host
  struct hostent *server;
  if ((server = gethostbyname(address)) == NULL)
    throw std::runtime_error("no such host");

  // Connect to Server
  struct sockaddr_in serv_addr = {};
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);

  bool success = false;
  for (int i = 0; i < PORT_ATTEMPTS; i++)
  {
    serv_addr.sin_port = htons(port + i);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
      printf("failed to connect to port %i\n", port + i);
    else
    {
      success = true;
      break;
    }
  }

  if (!success)
    throw std::runtime_error("failed to connect");

  // Start Listener Thread
  listener = (std::thread)[&]
  {
    int len;
    char buffer[256];

    while (running)
    {
      bzero(buffer, 256);
      if ((len = read(sockfd, buffer, sizeof(buffer))) < 0)
      {
        sleep(1);
        continue;
      }

      len--;
      buffer[len] = 0;

      Event event;
      event.type = Event::MESSAGE;
      event.sockfd = sockfd;
      event.content = std::string(buffer, len);

      callback(event);
    }
  };
}

Client::~Client() { close(sockfd); }

void Client::send(std::string msg)
{
  int n = write(sockfd, msg.data(), msg.size());
  if (n != msg.size())
    fprintf(stderr, "%s: failed to send message\n", __func__);
}