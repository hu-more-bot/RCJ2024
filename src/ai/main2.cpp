#include <iostream>
#include "portaudio.h"

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 64

// This is your audio buffer, you need to replace it with your actual audio data
// For demonstration purposes, I'll just use a simple sine wave
float audioBuffer[44100]; // Assuming 1 second of audio at 44100 Hz

// Callback function called by PortAudio when it needs more audio data
static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo *timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData)
{
    float *out = (float *)outputBuffer;
    unsigned int i;

    // Copy audio data from your buffer to the output buffer
    for (i = 0; i < framesPerBuffer; i++)
    {
        *out++ = audioBuffer[i]; // Assuming mono audio
    }

    return paContinue;
}

int main()
{
    PaError err;
    PaStream *stream;

    // Initialize PortAudio
    err = Pa_Initialize();
    if (err != paNoError)
    {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    // Open a stream for audio output
    err = Pa_OpenDefaultStream(&stream,
                               0,         // no input channels
                               1,         // mono output
                               paFloat32, // 32 bit floating point output
                               SAMPLE_RATE,
                               FRAMES_PER_BUFFER,
                               paCallback,
                               nullptr); // no user data
    if (err != paNoError)
    {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return 1;
    }

    // Start the stream
    err = Pa_StartStream(stream);
    if (err != paNoError)
    {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    // Wait for the user to press Enter before quitting
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    // Stop and close the stream
    err = Pa_StopStream(stream);
    if (err != paNoError)
    {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError)
    {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
    }

    // Terminate PortAudio
    Pa_Terminate();

    return 0;
}
