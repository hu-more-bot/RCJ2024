#include <sd-server.hpp>

#include <stdexcept>

static int callback(struct lws *wsi, enum lws_callback_reasons reason,
                    void *user, void *in, size_t len);

void *SDServer::service(void *arg)
{
    SDServer *self = (SDServer *)arg;

    pthread_detach(pthread_self());

    while (!self->quit)
    {
        lws_service(self->context, 0);
    }

    lws_context_destroy(self->context);

    pthread_exit(NULL);
    return NULL;
}

SDServer::SDServer(messageCallback onMessage, int port)
{
    struct lws_protocols protocols[] = {{"sd-server", callback, sizeof(void *), 0, 0, (void *)onMessage}, {}};

    struct lws_context_creation_info info = {};

    info.port = port;
    info.protocols = protocols;

    context = lws_create_context(&info);

    if (!context)
        throw std::runtime_error("SDServer: failed to init lws");

    quit = false;
    pthread_create(&thread, NULL, service, this);
}

SDServer::~SDServer()
{
    quit = true;
    pthread_join(thread, NULL);
}

static int callback(struct lws *wsi, enum lws_callback_reasons reason,
                    void *user, void *in, size_t len)
{
    SDServer::messageCallback onMessage = (SDServer::messageCallback)user;
    switch (reason)
    {
    case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
        printf("SDServer: new connection\n");
        break;

    case LWS_CALLBACK_RECEIVE:
        printf("SDServer: received data: %.*s\n", len, (char *)in);
        (*onMessage)((char *)in);
        break;

    default:
        break;
    }

    return 0;
}