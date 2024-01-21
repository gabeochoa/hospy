#pragma once

#include "base_component.h"

struct SnapsToSlot : public BaseComponent {

  virtual ~SnapsToSlot() {}

  int held_by = -1;
};
