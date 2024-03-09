//
// Created by samthedev on 11/9/23.
//

#pragma once

#include <cstdint>

namespace Artifex {

class Engine;

enum {
  ONCREATE = 1 << 0,
  ONDESTROY = 1 << 1,
  ONUPDATE = 1 << 2,
  ONFIXEDUPDATE = 1 << 2,
};

// Module
class Module {
public:
  Engine &engine;

public:
  Module(Engine &engine, uint32_t flags);
  ~Module();

  virtual bool onCreate() { return true; }
  virtual void onDestroy() {}
  virtual void onUpdate(float deltaTime) {}
};

} // namespace Artifex