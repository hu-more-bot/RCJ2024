#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>

#define NUM_BUFFERS 1
#define NUM_SOURCES 1

int main()
{
    ALuint buffer, source;
    ALsizei size, freq;
    ALenum format;
    ALvoid *data;

    // Initialize OpenAL
    ALCdevice *device = alcOpenDevice(NULL);
    if (!device)
    {
        std::cerr << "Failed to open OpenAL device" << std::endl;
        return 1;
    }

    ALCcontext *context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);

    // Generate buffers and sources
    alGenBuffers(NUM_BUFFERS, &buffer);
    alGenSources(NUM_SOURCES, &source);

    // Load your audio data into the buffer (Replace this with your own audio loading logic)
    // For simplicity, let's assume the audio data is already loaded into memory
    // Ensure 'data', 'size', 'freq', and 'format' are correctly set
    // For example, you can use a library like libsndfile to load audio files

    // Set the audio data properties
    alBufferData(buffer, format, data, size, freq);

    // Attach the buffer to the source
    alSourcei(source, AL_BUFFER, buffer);

    // Play the audio
    alSourcePlay(source);

    // Wait for the audio to finish playing
    ALint source_state;
    do
    {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    } while (source_state == AL_PLAYING);

    // Clean up resources
    alDeleteSources(NUM_SOURCES, &source);
    alDeleteBuffers(NUM_BUFFERS, &buffer);

    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return 0;
}
