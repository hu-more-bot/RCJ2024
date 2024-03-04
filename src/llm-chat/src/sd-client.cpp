#include <sd-client.hpp>

#include <stdexcept>

static int callback(struct lws *wsi, enum lws_callback_reasons reason,
                    void *user, void *in, size_t len);

void *SDClient::service(void *arg)
{
    SDClient *self = (SDClient *)arg;

    pthread_detach(pthread_self());

    while (!self->quit)
    {
        lws_service(self->context, 0);
    }

    lws_context_destroy(self->context);

    pthread_exit(NULL);
    return NULL;
}

SDClient::SDClient(int port, const char *address)
{
    // Create Context
    static struct lws_protocols protocols[] = {{"sd-server", callback, sizeof(void *) * 10, 0, 0, (void *)&text, 0}, {}};

    struct lws_context_creation_info info = {};

    info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
    info.protocols = protocols;
    // info.gid = -1;
    // info.uid = -1;

    if (!(context = lws_create_context(&info)))
        throw std::runtime_error("SDClient: failed to init lws");

    // Connect
    struct lws_client_connect_info i = {};

    i.protocol = i.local_protocol_name = "sd-server";
    i.address = i.host = i.origin = address;
    i.context = context;
    i.port = port;

    // i.path = "/";
    // i.host = lws_canonical_hostname(context);
    // i.origin = "origin";

    if (!(wsi = lws_client_connect_via_info(&i)))
        throw std::runtime_error("SDClient: failed to connect");

    quit = false;
    pthread_create(&thread, NULL, service, context);
}

SDClient::~SDClient()
{
    quit = true;
    pthread_join(thread, NULL);
}

void SDClient::send(std::string msg)
{
    text = msg;
    lws_callback_on_writable(wsi);
}

int callback(struct lws *wsi, enum lws_callback_reasons reason,
             void *user, void *in, size_t len)
{
    switch (reason)
    {
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
    {
        printf("SDClient: connection established\n");
    }
    break;

    case LWS_CALLBACK_CLIENT_WRITEABLE:
    {
        char buf[LWS_PRE + len];

        memcpy(&buf[LWS_PRE], in, len);
        lws_write(wsi, (unsigned char *)&buf[LWS_PRE], len, LWS_WRITE_TEXT);
    }
    break;

    case LWS_CALLBACK_CLIENT_CLOSED:
    {
        printf("SDClient: connection closed\n");
    }

    default:
        break;
    }

    return 0;
}
