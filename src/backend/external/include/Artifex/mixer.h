/**
 * @brief C Audio Player Library for Artifex
 *
 * @date 2024.06.xx TODO
 * @author SamTheDev
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Artifex Mixer Type
typedef struct axMixer_* axMixer;

// Check if Mixer is valid
int axMixerIsValid(axMixer mixer);

// Create Mixer
int axMixerCreate(axMixer* mixer, const char* deviceName);

// Destroy Mixer
void axMixerDestroy(axMixer* mixer);

// Load Audio
unsigned int axMixerLoad(axMixer mixer, int channels, int sample_rate, int samples, short* data);

// Unload Audio
void axMixerUnload(axMixer mixer, unsigned int audio);

// Start Playing Audio
unsigned int axMixerPlay(axMixer mixer, unsigned int audio, int isBlocking);

// Stop Playing Audio
int axMixerStop(axMixer mixer, unsigned int id);

// Get Audio State
int axMixerState(axMixer mixer, unsigned int id);

// Make OpenAL Context Current
void axMixerMakeCurrent(axMixer mixer);

// Get Audio Devices
int axMixerGetDevices();

// Check for OpenAL Errors
int axMixerCheckErrors();

#ifdef __cplusplus
}
#endif