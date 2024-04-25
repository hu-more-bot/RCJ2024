#include "pose.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>

bool Pose::save(const char *path) {
  FILE *f = fopen(path, "w");
  if (!f) {
    fprintf(stderr, "Failed to create pose file '%s'\n", path);
    return false;
  }

  fprintf(f, "// Poses\n");
  for (auto [name, data] : poses) {
    fprintf(f, "pose: %s\n", name.c_str());
    fprintf(f, "- left: %f %f %f %f\n", //
            data.left[0], data.left[1], data.left[2], data.left[3]);
    fprintf(f, "- right: %f %f %f %f\n", //
            data.left[0], data.left[1], data.left[2], data.left[3]);
  }

  fprintf(f, "\n// Anims\n");
  for (auto [name, data] : anims) {
    fprintf(f, "anim: %s\n", name.c_str());

    // Add Frames
    for (int i = 0; i < data.size(); i++) {
      const frame &frame = data[i];
      fprintf(f, "%i: %s %f\n", i, frame.pose.c_str(), frame.delay);
    }
  }

  return true;
}

bool Pose::exportBin(const char *path) {
  /*
  File Format:

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
  */

  FILE *f = fopen(path, "wb");
  if (!f)
    return false;

  // struct {
  //   char magic[4] = {'A', 'N', 'P', 'O'};

  //   uint8_t poses;
  //   uint32_t *pose[8];

  //   uint8_t anims;
  //   struct {
  //     uint8_t frames;
  //     struct {
  //       uint8_t poseID;
  //       float delay;
  //     } *frame;
  //   } *anim;
  // } bin;

  std::unordered_map<std::string, uint8_t> ids;

  // Write Magic
  if (fwrite("ANPO", 1, 4, f) != 4) {
    printf("failed to write magic\n");
    // goto xport_fail;
  }

  // Write Pose Count
  uint8_t poses = this->poses.size();
  if (fwrite(&poses, 1, 1, f) != 1) {
    printf("failed to write pose count\n");
    // goto xport_fail;
  }

  // Write Poses
  uint32_t pose[poses][8];
  {
    int i = 0;
    for (auto [key, p] : this->poses) {
      memcpy(pose[i], &p, 8 * 4);
      ids[key] = i;
      i += 1;
    }
  }
  if (fwrite(&pose, 8, poses, f) != poses) {
    printf("failed to write poses\n");
    // goto xport_fail;
  }

  // Write Anim Count
  uint8_t anims = this->anims.size();
  if (fwrite(&anims, 1, 1, f) != 1) {
    printf("failed to write anim count\n");
    // goto xport_fail;
  }

  // Write Anims
  for (auto [_, frame] : this->anims) {
    // Write Frame Count
    uint8_t frames = frame.size();
    if (fwrite(&frames, 1, 1, f) != 1) {
      printf("failed to write frame count\n");
      // goto xport_fail;
    }

    // Write Frames
    for (auto ff : frame) {
      // Write Frame ID
      uint8_t id = ids[ff.pose];
      if (fwrite(&id, 1, 1, f) != 1) {
        printf("failed to write frame id\n");
        // goto xport_fail;
      }

      // Write Frame Delay
      uint32_t delay = ff.delay;
      if (fwrite(&delay, 4, 1, f) != 1) {
        printf("failed to write frame delay\n");
        // goto xport_fail;
      }
    }
  }

  fclose(f);

  return true;

xport_fail:
  fclose(f);
  return false;
}

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

Pose Pose::importBin(const char *path) {
  Pose out;

  FILE *f = fopen(path, "rb");
  if (!f)
    return {};

  char magic[4];
  if (fread(magic, 1, 4, f) != 4 || strncmp(magic, "ANPO", 4)) {
    printf("Failed to read magic\n");
    return {};
  }

  // Read Poses
  uint8_t poses;
  fread(&poses, 1, 1, f);

  for (int i = 0; i < poses; i++) {
    pose_t p;
    fread(&p, 4, 8, f);
    out.poses[std::to_string(i)] = p;
  }

  // Read Anims
  uint8_t anims;
  fread(&anims, 1, 1, f);

  for (int i = 0; i < anims; i++) {
    uint8_t frames;
    fread(&frames, 1, 1, f);

    for (int j = 0; j < frames; j++) {
      uint8_t id;
      fread(&id, 1, 1, f);

      float delay;
      fread(&delay, 4, 1, f);

      out.anims[std::to_string(i)].push_back({std::to_string(id), delay});
    }
  }

  fclose(f);

  return out;
}