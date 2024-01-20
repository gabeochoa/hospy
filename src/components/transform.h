#pragma once

#include "../vec_util.h"
#include "../vendor_include.h"
#include "base_component.h"

struct Transform : public BaseComponent {
  virtual ~Transform() {}

  vec3 size = {1.f, 1.f, 1.f};
  vec3 position;

  [[nodiscard]] vec2 as2() const { return vec::to2(this->position); }

  /*
   * Get the bounding box for this entity
   * @returns BoundingBox the box
   * */
  [[nodiscard]] raylib::BoundingBox bounds() const {
    return get_bounds(this->position, this->size);
  }
};

inline std::ostream &operator<<(std::ostream &os, const Transform &t) {
  os << "Transform<> " << t.position << " " << t.size;
  return os;
}
