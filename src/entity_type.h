
#pragma once

#include <bitset>
#include <iostream>

#include "vendor_include.h"

enum struct EntityType { Unknown, x, y, z, Card, TraySlot };

using EntityTypeSet = std::bitset<magic_enum::enum_count<EntityType>()>;

inline std::string_view str(const EntityType &type) {
  return magic_enum::enum_name(type);
}

inline std::ostream &operator<<(std::ostream &os, const EntityType &type) {
  os << "EntityType: " << magic_enum::enum_name(type);
  return os;
}
