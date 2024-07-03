#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// C Asynchronous Audio Library
typedef struct AAudio *aaudio_t;

// Check if AAudio is healthy
int aaudioIsOK(aaudio_t aaudio);

// Create AAudio
int aauidoCreate(aaudio_t *aaudio, int frequency, int buffersize);

// Destroy AAudio
void aaudioDestroy(aaudio_t *aaudio);

// Start Listening
int aaudioStart(aaudio_t aaudio, unsigned long len);

// Stop Listening
void aaudioStop(aaudio_t aaudio);

// Clear Buffer
void aaudioClear(aaudio_t aaudio);

// Get n samples of audio, returns buffer length
unsigned long aaudioGet(aaudio_t aaudio, float *buf, unsigned long n);

#ifdef __cplusplus
}
#endif