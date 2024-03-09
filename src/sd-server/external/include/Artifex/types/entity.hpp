#pragma once

#include <cstdint>

#include <unordered_map>
#include <unordered_set>

enum ECSFLAG {
  ECSFLAG_DRAWABLE = 0,
  ECSFLAG_PHYSICS = 1,
};

struct Entity {
  std::unordered_set<ECSFLAG> flag;
  std::unordered_map<ECSFLAG, uint32_t> component;
};

// TODO: proper ECS, spacial partitioning system, etc