#pragma once

#include "pico/time.h"
#include "pico/types.h"
#include "servo.h"
#include <stdint.h>

#define LIMIT(x, min, max) (MAX(MIN(x, max), min))

struct Servo {
  int pin;

  float value;

  // Smoothing Amount (0 = disable)
  float smoothing;

  float prev;   // prev value (for smoothing)
  int min, max; // min & max safe pwm values
} servo[8];

typedef float pose[8];

// Animation
struct Animation {
  unsigned int frames;
  uint32_t *duration; // duration (ms) of every frame
  pose *pose;

  uint32_t start; // ms
};

void setPose(struct Servo servo[8], pose data);
void animate(struct Servo servo[8], struct Animation anim);
void setupServos(struct Servo servo[8], pose initialPose);

/**
 *
 * Implementations
 *
 */

void setPose(struct Servo servo[8], pose data) {
  for (int i = 0; i < 8; i++)
    servo[i].value = data[i];
}

void animate(struct Servo servo[8], struct Animation anim) {
  uint32_t now = to_ms_since_boot(get_absolute_time());

  // Start animation if not started
  if (anim.start == 0)
    anim.start = now;

  // Normalize time
  now = now - anim.start;

  // Iterate frames
  uint32_t total = 0;
  for (int i = 0; i < anim.frames; i++) {
    if (total + anim.duration[i] >= now) {
      setPose(servo, anim.pose[i]);
      total += anim.duration[i];
    }
  }
}

// TODO verify with at least one working brain cell

void setupServos(struct Servo servo[8], pose initialPose) {
  int min[8] = {350,  400, 600,  1300,  // left hand
                2400, 400, 2000, 1600}; // right hand

  int max[8] = {2400, 1400, 2000, 2600, // left hand
                350,  1400, 600,  350}; // right hand

  const int start_pin = 0;
  for (int i = 0; i < 8; i++) {
    struct Servo *s = &servo[i];

    s->pin = start_pin + i;
    servo_init(s->pin);

    // Set constants
    s->smoothing = 0.0003;

    s->min = min[i];
    s->max = max[i];
  }

  setPose(servo, initialPose);
}