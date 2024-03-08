#pragma once

#include <libwebsockets.h>

#include <pthread.h>

class SDServer {
public:
  typedef void (*messageCallback)(char *, int, void *);

  SDServer(messageCallback callback, void *user = NULL, int port = 8000);
  ~SDServer();

  int sockfd;

  void *user;
  messageCallback callback;

private:
  pthread_t accept;
};