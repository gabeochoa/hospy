#pragma once

#include "../vendor_include.h"
#include "base_component.h"

struct Transform : public BaseComponent {
  virtual ~Transform() {}

  vec3 size = {TILESIZE, TILESIZE, TILESIZE};
  vec3 position;

  [[nodiscard]] vec2 as2() const { return vec::to2(this->position); }
};

inline std::ostream &operator<<(std::ostream &os, const Transform &t) {
  os << "Transform<> " << t.position << " " << t.size;
  return os;
}
