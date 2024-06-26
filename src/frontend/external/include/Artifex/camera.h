/**
 * @brief C Camera Library for Artifex
 *
 * @date 2024.06.xx TODO
 * @author SamTheDev
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Artifex Camera Type
typedef struct axCamera_ *axCamera;

// RGB Frame Data
struct axCameraFrame {
  int width, height;
  unsigned char *data;
};

// Frame Callback Function
typedef void (*axCameraFrameCallback)(struct axCameraFrame *frame,
                                      void *user_ptr);

// Check if Camera is valid
int axCameraIsValid(axCamera camera);

// Create Camera
int axCameraCreate(axCamera *camera, int width, int height, int fps);

// Destroy Camera
void axCameraDestroy(axCamera *camera);

// Start RGB Camera Stream
int axCameraStart(axCamera camera, axCameraFrameCallback cb, void *user_ptr);

// Stop Camera Stream
void axCameraStop(axCamera camera);

#ifdef __cplusplus
}
#endif