#pragma once

#include "../vec_util.h"
#include "../vendor_include.h"
#include "base_component.h"

struct Transform : public BaseComponent {
  virtual ~Transform() {}

  vec2 size = {1.f, 1.f};
  vec2 position;
  float z_index = 0;

  void update(vec2 p) { position = p; }
  void init(vec2 p, vec2 sze, float z) {
    position = p;
    size = sze;
    z_index = z;
  }

  [[nodiscard]] vec2 as2() const { return this->position; }

  [[nodiscard]] raylib::Rectangle rect() const {
    return raylib::Rectangle{position.x, position.y, size.x, size.y};
  }
};

inline std::ostream &operator<<(std::ostream &os, const Transform &t) {
  os << "Transform<> " << t.position << " " << t.size;
  return os;
}
