#pragma once

// Pose
typedef float pose_t[8];

// Init Arms
int pose_init(int startPin);

// Set Target Pose
void pose_set(const pose_t pose);

// Update Arms
void pose_update();