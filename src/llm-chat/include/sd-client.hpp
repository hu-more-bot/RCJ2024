#pragma once

#include <libwebsockets.h>

#include <pthread.h>
#include <atomic>

#include <string>

class SDClient
{
public:
    SDClient(int port = 8000, const char *address = "localhost");
    ~SDClient();

    void send(std::string msg);

private:
    std::string text;

    struct lws_context *context;
    struct lws *wsi;

    pthread_t thread;
    static void *service(void *arg);

    std::atomic<bool> quit = false;
};