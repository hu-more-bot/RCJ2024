#include <sd-server.hpp>

#include <stdexcept>

#define SD_PROTOCOL "sd-server"

SDServer::SDServer(int port, const char *address)
{
    /* Create Context */ {
        static struct lws_protocols protocols[] = {{SD_PROTOCOL, callback, sizeof(void *) * 10, 0, 0, this, 0}, {}};

        struct lws_context_creation_info info = {};

        info.port = CONTEXT_PORT_NO_LISTEN; /* we do not run any server */
        info.protocols = protocols;
        info.gid = -1;
        info.uid = -1;

        if (!(context = lws_create_context(&info)))
            fprintf(stderr, "SD-Server: failed to create context\n");
    }

    struct lws_client_connect_info i = {};

    i.protocol = i.local_protocol_name = SD_PROTOCOL;
    i.address = i.host = i.origin = address;
    i.context = context;
    i.port = port;

    i.path = "/";
    i.host = lws_canonical_hostname(context);
    i.origin = "origin";

    if (!(wsi = lws_client_connect_via_info(&i)))
        throw std::runtime_error("Failed to connect to SD Server");
}

SDServer::~SDServer()
{
    lws_context_destroy(context);
}

void SDServer::update()
{
    lws_service(context, 250);
}

void SDServer::send(std::string msg)
{
    text = msg;
    lws_callback_on_writable(wsi);
}

int SDServer::callback(struct lws *wsi, enum lws_callback_reasons reason,
                       void *user, void *in, size_t len)
{
    SDServer *sd = (SDServer *)user;
    switch (reason)
    {
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
    {
        printf("SD-Server: connection established\n");
    }
    break;

    case LWS_CALLBACK_CLIENT_WRITEABLE:
    {
        const char *msg = "hello";
        const size_t size = strlen(msg);
        char buf[LWS_PRE + size];

        memcpy(&buf[LWS_PRE], msg, size);
        printf("Sending: %s\n", sd->text.c_str());
        lws_write(wsi, (unsigned char *)&buf[LWS_PRE], size, LWS_WRITE_TEXT);
    }
    break;

    case LWS_CALLBACK_CLIENT_CLOSED:
    {
        printf("SD-Server: connection closed\n");
    }

    default:
        break;
    }

    return 0;
}
