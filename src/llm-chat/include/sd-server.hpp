#pragma once

#include <libwebsockets.h>

#include <string>

class SDServer
{
public:
    SDServer(int port = 8000, const char *address = "localhost");
    ~SDServer();

    void update();
    void send(std::string msg);

private:
    std::string text;

    static int callback(struct lws *wsi, enum lws_callback_reasons reason,
                        void *user, void *in, size_t len);

    struct lws_context *context{};
    struct lws *wsi{};
};