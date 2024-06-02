#include <server.hpp>

#include <llm.hpp>
#include <sd.hpp>

int main()
{
    printf("Loading LLM...\n");
    LLM llm(getenv("MODEL_ZEPHYR_7B_BETA"), "../prompt.txt");

    printf("Loading SD...\n");
    SD sd(getenv("MODEL_SD"));
    std::thread painter; // image generator thread

    auto callback = [&](Server &server, const Server::Event &event)
    {
        if (event.type != Server::Event::MESSAGE)
            return;

        if (!strncmp(event.data, "TXIN", 4))
        {
            // Decode Message & Generate Response
            llm.decode("client", std::string(event.data + 4, event.len));
        }
        else if (!strncmp(event.data, "IMTX", 4))
        {
            // Generate Image from prompt

            struct
            {
                int width;
                int height;
                char *prompt;
            } msg;

            memcpy(&msg, event.data + 4, sizeof(int) * 2);
            msg.prompt = event.data + 4 + sizeof(int) * 2;

            // Start Painter Thread
            painter = (std::thread)[&]
            {
                sd.config.width = msg.width;
                sd.config.height = msg.height;
                sd.config.seed = time(0);

                strcpy(sd.config.prompt, msg.prompt);

                if (!sd.generate())
                {
                    printf("failed to generate image\n");
                }

                // TODO compress & send image
                // format: "IM[ch][size][IMAGE DATA]"
                char out[2 + sizeof(int) * 3 + sd.result->channel * sd.result->width * sd.result->height];
                memcpy(out, "IM", 2);
                memcpy(out + 2, &sd.result->channel, sizeof(int));
                memcpy(out + 2 + sizeof(int), &sd.result->width, sizeof(int));
                memcpy(out + 2 + sizeof(int) * 2, &sd.result->height, sizeof(int));

                server.send(event.sockfd, out);
            };
        }
        else if (!strncmp(event.data, "IMIN", 4))
        {
            // Generate Image from prompt using image
        }
    };

    printf("Starting Server...\n");
    Server server(8000, callback);

    while (true)
    {
        // Get User In
        printf("You:\n\e[0;92m");
        char text[256];
        memset(text, 0, sizeof(text));
        fgets(text, sizeof(text), stdin);
        printf("\e[39m");

        llm.decode("client", std::string(text));

        // Print AI Out
        printf("AI:\n\e[0;36m");

        llm.generate([&](std::string token)
                     { printf("%s", token.c_str());
                     fflush(stdout); });

        printf("\e[39m");
    }
}