#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Speech-To-Text
typedef struct STT *stt_t;

// typedef void (*sttCallback)(const struct sttEvent *, void *);

// Check if stt is healthy
int sttIsOK(stt_t stt);

// Create STT
int sttCreate(stt_t *stt, const char *path);

// Destroy STT
void sttDestroy(stt_t *stt);

// Listen
int sttListen(stt_t stt, char *text, unsigned long len);

#ifdef __cplusplus
}
#endif