#pragma once

#include <pose.h>
#include <servo.h>

#include <pico/time.h>

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define LIMIT(x, min, max) (MAX(MIN(x, max), min))
#define PERCENT(x) (LIMIT(x, 0, 1))

#define SMOOTHING 0.01

// Pose Config
struct {
  const int min[8];
  const int max[8];

  int startPin;

  pose_t prev, target;
  float now, past;

  int isInit;
} _pose = {{350, 400, 600, 1300,    // left hand
            2400, 400, 2000, 1600}, // right hand
           {2400, 1400, 2000, 2600, // left hand
            350, 1400, 600, 350},   // right hand
           0,
           {},
           {},
           0,
           0,
           0};

int pose_init(int startPin) {
  if (_pose.isInit)
    return 1;

  _pose.startPin = startPin;
  for (int i = 0; i < 8; i++)
    servo_init(_pose.startPin + i);

  _pose.isInit = 1;

  _pose.now = _pose.past = to_ms_since_boot(get_absolute_time()) / 1000.0f;

  return 0;
}

void pose_set(pose_t pose) {
  for (int i = 0; i < 8; i++)
    _pose.target[i] = pose[i];
}

void pose_update() {
  // Calculate DeltaTime
  _pose.past = _pose.now,
  _pose.now = to_ms_since_boot(get_absolute_time()) / 1000.0f;
  float delta = _pose.now - _pose.past;

  for (int i = 0; i < 8; i++) {

    float value = _pose.target[i]; // %
    float min = _pose.min[i], max = _pose.max[i];

    // Smooth out movement
    if (SMOOTHING > 0) {
      const float s = SMOOTHING * delta;
      // pwm = (pwm * s_amount) + (prev * (100% - s_amount))
      value = (value * s) + (_pose.prev[i] * (1.0 - s));
      _pose.prev[i] = value;
    }

    // pwm = low + % * (high-low)
    servo_setMillis(_pose.startPin + i, min + PERCENT(value) * (max - min));
  }
}