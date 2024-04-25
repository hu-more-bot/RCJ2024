#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct pose_t {
  float left[4], right[4];
};

/*

ASCII File Format:
```.pose
// comment

// new pose
pose: a
- left: 0 0 1 0
- right: 0 0 1 0

pose: b
- left: 0.3 0.0 0.2 0.0
- right: 0.3 0.0 0.2 0.0

// new animation
anim: idle

// set frames
- frames:
0: {0 0 0 0} {0 0 0 0} 1
1: a 1.7
2: b 1.2
```

Binary File Format:
```.bpose
(4 byte) char magic ("ANPO")

(1 byte) unsigned int pose_count
(x * 8 * 4 byte) float[8] poses

(1 byte) unsigned int anim_count
(y * 4 * z byte) [
  (1 byte) unsigned int frame_count
  (z * 8 byte) [
    (1 byte) unsigned int pose id
    (4 byte) float delay
  ] frames
] anims
```

*/

struct Pose {
  std::unordered_map<std::string, pose_t> poses;

  struct frame {
    std::string pose;
    float delay;
  };

  std::unordered_map<std::string, std::vector<frame>> anims;

  // Save Pose
  bool save(const char *path);

  // Export Binary File
  bool exportBin(const char *path);

  // Load Pose from file
  static Pose load(const char *path);

  // Import Binary File
  static Pose importBin(const char *path);
};