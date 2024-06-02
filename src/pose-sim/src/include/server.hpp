#pragma once

#include <functional>
#include <pthread.h>

// Borrowed from sd-server/SDServer
class Server
{
public:
  Server(const std::function<void(char *, int)> &callback, int port = 8000);
  ~Server();

  int m_sockfd;

private:
  pthread_t m_accept;
  const std::function<void(char *, int)> m_callback;

  int m_port;
};