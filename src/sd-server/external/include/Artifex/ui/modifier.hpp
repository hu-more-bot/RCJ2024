//
// Created by samthedev on 11/13/23.
//

#pragma once

#include <Artifex/math/vec.hpp>

#include <string>
#include <unordered_map>

namespace Artifex::UI {

struct Modifier {
public:
  // New Empty Modifier
  Modifier();

  // Load Modifier Chain
  explicit Modifier(const std::string &id);
  Modifier load(const std::string &id);

  // Save Modifier Chain
  Modifier save(const std::string &id);

  // TODO

private:
  static std::unordered_map<std::string, Modifier> modifiers;
};

} // namespace Artifex::UI
