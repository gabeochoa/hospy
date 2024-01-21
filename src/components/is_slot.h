
#pragma once

#include "base_component.h"

struct IsSlot : public BaseComponent {
  virtual ~IsSlot() {}

  int held_entity = -1;

  [[nodiscard]] bool is_empty() const { return held_entity == -1; }
};
