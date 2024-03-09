//
// Created by samthedev on 11/13/23.
//

#pragma once

#include "modifier.hpp"
#include <vector>

namespace Artifex::UI {

struct Element {
  vec<2> center, position, size;
  float orientation;
};

struct Layout {
public:
  // UI Elements
  std::vector<Element> element;

  // UI ELement States
  std::unordered_map<std::string, uint8_t> state;
};

} // namespace Artifex::UI
