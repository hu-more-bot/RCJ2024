#include <yolo.h>

#include <Artifex/log.h>

#include <onnxruntime_c_api.h>

#include <stdio.h>
#include <stdlib.h>

#define TAG "yolo"

struct Yolo {
  const OrtApi *ort;
  OrtEnv *env;
  OrtSessionOptions *options;
  OrtSession *session;

  size_t len;
  char **params;
};

int yoloIsOK(yolo_t yolo) {
  if (!yolo)
    return 0;

  return 1;
}

int yoloCreate(yolo_t *yolo, const char *path, const char *paramsPath) {
  if (!yolo)
    return 1;

  yolo_t ylo = *yolo = malloc(sizeof(struct Yolo));
  if (!ylo) {
    ax_error(TAG, "failed to allocate memory");
    return 1;
  }

  ylo->ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
  if (!ylo->ort) {
    free(*yolo);
    *yolo = NULL;

    ax_error(TAG, "failed to init onnx runtime engine: %s");
    return 1;
  }

  if (ylo->ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "ONNX", &ylo->env)) {
    free(*yolo);
    *yolo = NULL;

    ax_error(TAG, "failed to create onnx environment");
    return 1;
  }

  if (ylo->ort->CreateSessionOptions(&ylo->options)) {
    ylo->ort->ReleaseEnv(ylo->env);
    free(*yolo);
    *yolo = NULL;

    ax_error(TAG, "failed to create onnx session options");
    return 1;
  }

  if (ylo->ort->CreateSession(ylo->env, path, ylo->options, &ylo->session)) {
    ylo->ort->ReleaseSessionOptions(ylo->options);
    ylo->ort->ReleaseEnv(ylo->env);
    free(*yolo);
    *yolo = NULL;

    ax_error(TAG, "failed to create onnx session (file?)");
    return 1;
  }

  // Load Params
  FILE *f = fopen(paramsPath, "r");
  if (!f) {
    ylo->ort->ReleaseSession(ylo->session);
    ylo->ort->ReleaseSessionOptions(ylo->options);
    ylo->ort->ReleaseEnv(ylo->env);
    free(*yolo);
    *yolo = NULL;

    ax_error(TAG, "failed to open config file");
    return 1;
  }

  char line[32];
  fgets(line, sizeof(line), f);

  // Read length
  if (sscanf(line, "%zu", &ylo->len) != 1 || ylo->len == 0) {
    fclose(f);
    ylo->ort->ReleaseSession(ylo->session);
    ylo->ort->ReleaseSessionOptions(ylo->options);
    ylo->ort->ReleaseEnv(ylo->env);
    free(*yolo);
    *yolo = NULL;

    ax_error(TAG, "failed to read config file");
    return 1;
  }

  ylo->params = malloc(sizeof(char *) * ylo->len);

  for (int i = 0; i < ylo->len; i++) {
    ylo->params[i] = malloc(32);

    // read line
    fgets(line, sizeof(line), f);
    strcpy(ylo->params[i], line);
  }

  ax_verbose(TAG, "initialized");
  return 0;
}

void yoloDestroy(yolo_t *yolo) {
  if (!yolo || !*yolo)
    return;

  for (size_t i = 0; i < (*yolo)->len; i++)
    free((*yolo)->params[i]);
  free((*yolo)->params);

  (*yolo)->ort->ReleaseSessionOptions((*yolo)->options);
  (*yolo)->ort->ReleaseSession((*yolo)->session);
  (*yolo)->ort->ReleaseEnv((*yolo)->env);

  free(*yolo);
  *yolo = NULL;

  ax_verbose(TAG, "destroyed");
}

unsigned long yoloSize(yolo_t yolo) {
  if (!yoloIsOK(yolo))
    return 0;

  OrtTypeInfo *typeInfo;
  if (yolo->ort->SessionGetInputTypeInfo(yolo->session, 0, &typeInfo)) {
    // TODO
  }

  const OrtTensorTypeAndShapeInfo *tensorInfo;
  if (yolo->ort->CastTypeInfoToTensorInfo(typeInfo, &tensorInfo)) {
    // TODO
  }

  unsigned long size;
  if (yolo->ort->GetDimensionsCount(tensorInfo, &size)) {
    // TODO
  }

  long dims[size];
  if (yolo->ort->GetDimensions(tensorInfo, dims, size)) {
    // TODO
  }

  yolo->ort->ReleaseTypeInfo(typeInfo);

  return dims[size - 1];
}

unsigned long yoloDetect(yolo_t yolo, float *data, struct Result **results) {
  if (!results) {
    // TODO shouldn't be NULL
  }

  // Create Memory Info
  OrtMemoryInfo *memory_info;
  if (yolo->ort->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault,
                                     &memory_info)) {
    // TODO failed to create memory info
  }

  // Create Input Tensor
  long shape[] = {1, 3, 640, 640};

  OrtValue *input = NULL;
  if (yolo->ort->CreateTensorWithDataAsOrtValue(
          memory_info, data, sizeof(float) * 1 * 640 * 640 * 3, shape, 4,
          ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT, &input)) {
    // TODO failed to create tensor
  }

  // Free Memory Info
  yolo->ort->ReleaseMemoryInfo(memory_info);

  // Run Inference
  const char *input_names[] = {"images"};
  const char *output_names[] = {"output"};

  OrtValue *output = NULL;
  if (yolo->ort->Run(yolo->session, NULL, input_names,
                     (const OrtValue *const *)&input, 1, output_names, 1,
                     &output)) {
    // TODO failed to run
  }

  OrtTensorTypeAndShapeInfo *info;
  if (yolo->ort->GetTensorTypeAndShape(output, &info)) {
    // TODO failed to get type/shape
  }

  unsigned long size;
  if (yolo->ort->GetTensorShapeElementCount(info, &size)) {
    // TODO failed to get size
  }

  float *ptr = NULL;
  if (yolo->ort->GetTensorMutableData(output, (void **)&ptr)) {
    // TODO failed to get data
  }

  // Get Dimensions / Shape
  unsigned long count = 0, elements = 0;
  /* Get Dimensions */ {
    unsigned long size;
    if (yolo->ort->GetDimensionsCount(info, &size)) {
      // TODO
    }

    if (size != 2) {
      // TODO should be 2
    }

    long dims[size];
    if (yolo->ort->GetDimensions(info, dims, size)) {
      // TODO
    }

    count = dims[0];
    elements = dims[1];

    if (elements != 7) {
      // TODO should be 7
    }
  }

  *results = malloc(sizeof(struct Result) * count);
  for (int i = 0; i < count; i++) {
    (*results)[i].x = ptr[1];
    (*results)[i].y = ptr[2];
    (*results)[i].w = ptr[3];
    (*results)[i].h = ptr[4];
    (*results)[i].index = ptr[5];

    if ((*results)[i].index < yolo->len)
      (*results)[i].name = yolo->params[(*results)[i].index];
    else
      (*results)[i].name = "";

    (*results)[i].confidence = ptr[6];

    ptr += elements;
  }

  // Free Values
  yolo->ort->ReleaseValue(output);
  yolo->ort->ReleaseValue(input);

  return count;
}