#pragma once

#include "base_component.h"

struct IsDraggable : public BaseComponent {
  virtual ~IsDraggable() {}

  bool is_selected;
};
