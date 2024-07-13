#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Simple YOLO Instance
typedef struct Yolo *yolo_t;

/// This class represents a detection instance
struct Result {
  float x, y, w, h;

  int index;
  const char *name;

  double confidence;
};

/// List of detected instances in an image

// Check if yolo is healthy
int yoloIsOK(yolo_t yolo);

// Create YOLO
int yoloCreate(yolo_t *yolo, const char *path, const char *paramsPath);

// Destroy YOLO
void yoloDestroy(yolo_t *yolo);

// Get Image Size
unsigned long yoloSize(yolo_t yolo);

/// Run inference
/// @param yolo YOLO instance
/// @param data RGB rectangular data of returned size
/// @param results array of results
/// @returns size of array
/// @warning result array should be free-d
unsigned long yoloDetect(yolo_t yolo, float *data, struct Result **results);

#ifdef __cplusplus
}
#endif