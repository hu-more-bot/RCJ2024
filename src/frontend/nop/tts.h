/**
 * @brief C TTS API based on Piper
 * @note This library is implemented based on piper.cpp, therefore I don't claim
 * ownership of it. All credits go to @rhasspy, the original author of piper
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