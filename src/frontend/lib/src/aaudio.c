#include <aaudio.h>

#include <AL/al.h>
#include <AL/alc.h>

#include <stdlib.h>
#include <string.h>

#include <Artifex/log.h>

#define TAG "aaudio"

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

struct AAudio {
  ALCdevice *device;

  // Circular Recording Buffer
  float *buffer;
  unsigned long size, cursor, len;
};

int aaudioIsOK(aaudio_t aaudio) {
  if (!aaudio || !aaudio->device)
    return 0;
  return 1;
}

int aauidoCreate(aaudio_t *aaudio, int frequency, int buffersize) {
  if (!aaudio)
    return 1;

  aaudio_t audio = *aaudio = malloc(sizeof(struct AAudio));
  if (!audio) {
    ax_error(TAG, "failed to allocate memory");
    return 1;
  }

  // Open Device
  audio->device =
      alcCaptureOpenDevice(NULL, frequency, AL_FORMAT_MONO16, buffersize);

  if (alGetError() != AL_NO_ERROR || !audio->device) {
    free(audio);
    audio = NULL;

    ax_error(TAG, "failed to open device");
    return 0;
  }

  ax_verbose(TAG, "initialized");
  return 0;
}

void aaudioDestroy(aaudio_t *aaudio) {
  if (!aaudio || !*aaudio)
    return;

  aaudioStop(*aaudio);

  free(*aaudio);
  *aaudio = NULL;

  ax_verbose(TAG, "destroyed");
}

int aaudioStart(aaudio_t aaudio, unsigned long len) {
  if (!aaudioIsOK(aaudio))
    return 1;

  // TODO
  return 0;
}

void aaudioStop(aaudio_t aaudio) {
  if (!aaudioIsOK(aaudio))
    return;

  // TODO
}

void aaudioClear(aaudio_t aaudio) {
  if (!aaudioIsOK(aaudio))
    return;

  aaudio->cursor = aaudio->len = 0;
}

unsigned long aaudioGet(aaudio_t aaudio, float *buf, unsigned long n) {
  if (!aaudioIsOK(aaudio))
    return 0;

  // remaining len until buffer end
  unsigned long remaining = aaudio->size - aaudio->cursor;

  memcpy(buf, aaudio->buffer + aaudio->cursor,
         MIN(MIN(aaudio->len, remaining), n));

  // copy second part
  //   if (remaining < MIN(aaudio->len, n))
  // memcpy(buf + (n - remaining))

  // TODO

  return MIN(aaudio->len, n);
}