#pragma once

#include <pose.h>

#include <math.h>

// Animation
typedef struct {
  size_t poses;
  pose_t *pose;

  size_t frames;
  size_t *frame;
  float *delay;
} anim_t;

// Initialize Pose & Animations
int anim_init(int startPin, anim_t *anim);

// Set Animation
void anim_set(size_t id);

// Play Animation
void anim_update();