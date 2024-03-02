#include <al.hpp>

#include <stdexcept>

AL::AL()
{
    // Initialize OpenAL
    if (!(device = alcOpenDevice(NULL)))
        throw std::runtime_error(std::string(__func__) + ": failed to open device");

    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);
}

AL::~AL()
{
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

size_t AL::add(std::vector<int16_t> &data, int freq)
{
    // Audio properties
    ALsizei size = data.size() * sizeof(int16_t); // Size of audio data in bytes
    // ALsizei freq = (float)44100 / (float)2;       // Sample frequency, adjust according to your audio data
    ALenum format = AL_FORMAT_MONO16; // Monophonic 16-bit PCM format

    // Generate buffer and source
    ALuint buffer;
    alGenBuffers(1, &buffer);

    // Load audio data into buffer
    alBufferData(buffer, format, data.data(), size, freq);

    // Check for OpenAL errors
    ALenum alError = alGetError();
    if (alError != AL_NO_ERROR)
        throw std::runtime_error(std::string(__func__) + ": OpenAL error");

    buffers.push_back(buffer);
    return buffers.size() - 1;
}

void AL::remove(size_t id)
{
    alDeleteBuffers(1, &buffers[id]);
    buffers.erase(buffers.begin() + id);
}

void AL::play(size_t id, bool wait)
{
    // Play the audio
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffers[id]);
    alSourcePlay(source);

    // Wait for the audio to finish playing
    ALint source_state;
    do
    {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    } while (source_state == AL_PLAYING); // && wait);

    // Clean up resources
    alDeleteSources(1, &source);
}