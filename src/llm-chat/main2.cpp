#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>

#include <tts.hpp>

int main()
{
    TTS tts("../models/piper/kusal.onnx");

    auto audioData = tts.say("pfjhojpohahdofklhalmfkbjhapgfouihaponryakgbfdantrmlhknd hnt,mhndfkljhnfhkbdl hakjt nrea ;idlhjdflhjdlkgjhlkhkhjgflkh.,ljhtehre;lgnfsglkg jadhk;lghkgh;lkghjgs;lkjhnglfhga;l.hmgdl;'hil tlkfdghare");

    // Audio properties
    ALsizei size = audioData.size() * sizeof(int16_t); // Size of audio data in bytes
    ALsizei freq = (float)44100 / (float)2;            // Sample frequency, adjust according to your audio data
    ALenum format = AL_FORMAT_MONO16;                  // Monophonic 16-bit PCM format

    // Initialize OpenAL
    ALCdevice *device = alcOpenDevice(NULL);
    if (!device)
    {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return 1;
    }

    ALCcontext *context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);

    // Generate buffer and source
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // Load audio data into buffer
    alBufferData(buffer, format, audioData.data(), size, freq);

    // Check for OpenAL errors
    ALenum alError = alGetError();
    if (alError != AL_NO_ERROR)
    {
        std::cerr << "OpenAL error: " << alError << std::endl;
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
        return 1;
    }

    // Play the audio
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcePlay(source);

    // Wait for the audio to finish playing
    ALint source_state;
    do
    {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    } while (source_state == AL_PLAYING);

    // Clean up resources
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);

    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}
