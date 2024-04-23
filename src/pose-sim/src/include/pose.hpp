#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct pose_t {
  float left[4], right[4];
};

struct Pose {
  std::unordered_map<std::string, pose_t> poses;

  struct frame {
    std::string pose;
    float delay;
  };

  std::unordered_map<std::string, std::vector<frame>> anims;

  // Save Pose
  bool save(const char *path);

  // Load Pose from file
  static Pose load(const char *path);
};