#pragma once

#include <pose.h>

#include <stdint.h>

// Animation
typedef struct {
  uint8_t poses;
  pose_t *pose;

  uint8_t frames;
  struct frame_t {
    uint8_t pose;
    float delay;
  } *frame;
} anim_t;

// Initialize Pose & Animations
int anim_init(int startPin, anim_t *anim);

// Set Animation
void anim_set(uint8_t id);

// Play Animation
void anim_update();