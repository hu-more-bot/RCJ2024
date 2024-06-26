/**
 * @brief C TTS API for Piper
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Piper C API
typedef struct TTS *tts_t;

// Check if tts is healthy
int ttsIsOK(tts_t tts);

// Create tts
int ttsCreate(tts_t *tts, const char *path);

// Destroy tts
void ttsDestroy(tts_t *tts);

#ifdef __cplusplus
}
#endif