#include <al.hpp>
#include <stt.hpp>
#include <tts.hpp>

#include <client.hpp>

#define SPEECH_INPUT false

int main()
{
    printf("Loading TTS...\n");
    TTS tts(getenv("MODEL_PIPER"), "amy");

#if SPEECH_INPUT
    printf("Loading STT...\n");
    STT stt(getenv("MODEL_WHISPER_BASE"));
#endif

    auto callback = [&](const Client::Event &event)
    {
        if (event.type != Client::Event::MESSAGE)
            return;

        // TODO
    };

    printf("Starting Client...\n");
    Client client(8000, "localhost", callback);

    while (true)
    {
        // Wait for user input
        printf("User In: \n");
#if SPEECH_INPUT
        std::string user_in_s = stt.listen();
        char user_in[128];
        printf("Said: '%s'\n", user_in);
#else
        char user_in[128];
        memset(user_in, 0, sizeof(user_in));
        fgets(user_in, sizeof(user_in), stdin);
#endif

        if (!strncasecmp(user_in, "exit", 4))
            break;

        // Handle 0-length responses
        if (response.size() == 0)
            response = "One minute please...";

        // Generate Response Audio
        auto buf = tts.say(response);
        al.add(buf, 44100 / 2);

        // Process Commands
        printf("commands: %zu\n", commands.size());
        for (auto c : commands)
        {
            // printf("%s\n", c.c_str());
            sd.send(c);
            // poseSim.send("idle");
        }

        // Play Response Audio
        al.play(0);
        al.remove(0);
    }

    return 0;
}