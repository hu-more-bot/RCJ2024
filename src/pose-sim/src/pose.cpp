#include "pose.hpp"

#include <cstring>

Pose Pose::load(const char *path) {
  Pose pose;

  FILE *f = fopen(path, "r");
  if (!f) {
    fprintf(stderr, "Failed to open pose file '%s'\n", path);
    return {};
  }

  struct {
    enum { NONE, POSE, ANIM } type;
    std::string name, meta;
  } current;

  char line[128];
  while (fgets(line, sizeof(line), f)) {
    // Determine Line Type
    char type[8], data[120];
    int c = sscanf(line, "%7[^ ] %119[^\n]", type, data);

    if (!strncmp(type, "//", 2) || c < 2) {
      // Comment or empty; ignore
    } else if (!strncmp(type, "pose", 4)) {
      // New Pose
      if (pose.poses.count(data) != 0) {
        fprintf(stderr, "Pose '%s' already exists\n", data);
        current.type = current.NONE;
        continue;
      }

      current.type = current.POSE;
      current.name = data;
      pose.poses[data] = {};
    } else if (!strncmp(type, "anim", 4)) {
      // New Animation
      if (pose.anims.count(data) != 0) {
        fprintf(stderr, "Anim '%s' already exists\n", data);
        current.type = current.NONE;
        continue;
      }

      current.type = current.ANIM;
      current.name = data;
      pose.anims[data] = {};
    } else if (!strncmp(type, "-", 1)) {
      // Property
      char prop[16], subdata[100];
      c = sscanf(data, "%15[^ ] %99[^\n]", prop, subdata);

      switch (current.type) {
      case current.POSE: {
        if (c != 2) {
          fprintf(stderr, "Invalid data for prop '%s'\n", prop);
          continue;
        }

        int side = 0;

        if (!strncmp(prop, "left", 4))
          side = 0;
        else if (!strncmp(prop, "right", 5))
          side = 1;
        else
          goto no_prop;

        float v[4]{};
        c = sscanf(subdata, "%f %f %f %f", &v[0], &v[1], &v[2], &v[3]);

        for (int i = 0; i < 4; i++)
          (side == 0 ? pose.poses[current.name].left
                     : pose.poses[current.name].right)[i] = v[i];
      } break;

      case current.ANIM: {
        if (!strncmp(prop, "frames", 5)) {
          // Frames
          current.meta = "frames";
        } else
          goto no_prop;
      } break;

      default:
      case current.NONE:
      no_prop:
        fprintf(stderr, "Unknown Property: '%s'\n", prop);
        continue;
        break;
      }
    } else {
      switch (current.type) {
      default:
      case current.ANIM:
        if (current.meta == "frames") {
          // TODO
          pose_t p;
          float delay;
          char name[16] = "_frame";

          c = sscanf(data, "{%f %f %f %f} {%f %f %f %f} %f",         //
                     &p.left[0], &p.left[1], &p.left[2], &p.left[3], //
                     &p.right[0], &p.right[1], &p.right[2], &p.right[3],
                     &delay);
          if (c != 9) {
            c = sscanf(data, "%s %f", name, &delay);
            if (c != 2) {
              fprintf(stderr, "Invalid frame");
              continue;
            }
            pose.anims[current.name].push_back((Pose::frame){name, delay});
          } else {
            pose.poses[name] = p;
            pose.anims[current.name].push_back((Pose::frame){name, delay});
          }
        } else {
          goto no_param;
        }
        break;
      case current.NONE:
      case current.POSE:
      no_param:
        // Unknown
        fprintf(stderr, "Unrecognised parameter: '%s'\n", type);
        break;
      }
    }
  }

  return pose;
}