
#pragma once

#include "../vendor_include.h"
#include "base_component.h"
#include "magic_enum/magic_enum.hpp"
#include <bitset>

enum RenderTagType {
  None,
  Highlight,
};

using RenderTagSet = std::bitset<magic_enum::enum_count<RenderTagType>()>;

struct RenderTags : public BaseComponent {
  RenderTagSet tags;

  RenderTags() { tags.reset(); }

  [[nodiscard]] int index(RenderTagType type) const {
    return magic_enum::enum_integer<RenderTagType>(type);
  }
  void enable_tag(RenderTagType type) { tags.set(index(type)); }
  void disable_tag(RenderTagType type) { tags.reset(index(type)); }

  [[nodiscard]] bool has_tag(RenderTagType type) const {
    return tags.test(magic_enum::enum_integer<RenderTagType>(type));
  }

  [[nodiscard]] bool missing_tag(RenderTagType type) const {
    return !has_tag(type);
  }

  virtual ~RenderTags() {}
};
