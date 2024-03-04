#pragma once

#include <libwebsockets.h>

#include <pthread.h>
#include <atomic>

class SDServer
{
public:
    typedef void (*messageCallback)(char *);

    SDServer(messageCallback onMessage, int port = 8000);
    ~SDServer();

private:
    struct lws_context *context;

    pthread_t thread;
    static void *service(void *arg);

    std::atomic<bool> quit = false;
};