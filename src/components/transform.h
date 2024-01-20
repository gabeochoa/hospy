#pragma once

#include "../vec_util.h"
#include "../vendor_include.h"
#include "base_component.h"

struct Transform : public BaseComponent {
  virtual ~Transform() {}

  vec2 size = {1.f, 1.f};
  vec3 position;

  [[nodiscard]] vec2 as2() const { return vec::to2(this->position); }
  void update(vec3 p) { position = p; }
  void init(vec3 p, vec2 sze) {
    position = p;
    size = sze;
  }

  /*
   * Get the bounding box for this entity
   * @returns BoundingBox the box
   * */
  [[nodiscard]] raylib::BoundingBox bounds() const {
    return get_bounds(this->position, vec::to3(this->size));
  }
};

inline std::ostream &operator<<(std::ostream &os, const Transform &t) {
  os << "Transform<> " << t.position << " " << t.size;
  return os;
}
