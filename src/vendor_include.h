#pragma once

// this has to be before the raylib thing
// because otherwise nullptr and int and whatnot
// will be inside the namespace
#include "std_include.h"

#include "engine/graphics.h"

#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic ignored "-Wdeprecated-volatile"
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

#ifdef WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <fmt/ostream.h>
// this is needed for wstring printing
#include <fmt/xchar.h>
//
#include <expected.hpp>
//

#include <magic_enum/magic_enum.hpp>
// TODO :INFRA: cant use format yet due to no std::format yet (though not even
// sure what its needed for) #include <magic_enum/magic_enum_format.hpp>
#include <magic_enum/magic_enum_fuse.hpp>
#include <nlohmann/json.hpp>

#ifdef __APPLE__
#pragma clang diagnostic pop
#else
#pragma enable_warn
#endif

#ifdef WIN32
#pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

// For bitwise operations
template <typename T>
constexpr auto operator~(T a) ->
    typename std::enable_if<std::is_enum<T>::value, T>::type {
  return static_cast<T>(~static_cast<int>(a));
}

template <typename T>
constexpr auto operator|(T a, T b) ->
    typename std::enable_if<std::is_enum<T>::value, T>::type {
  return static_cast<T>((static_cast<int>(a) | static_cast<int>(b)));
}

template <typename T>
constexpr auto operator&(T a, T b) ->
    typename std::enable_if<std::is_enum<T>::value, T>::type {
  return static_cast<T>((static_cast<int>(a) & static_cast<int>(b)));
}

template <typename T>
constexpr auto operator^(T a, T b) ->
    typename std::enable_if<std::is_enum<T>::value, T>::type {
  return static_cast<T>((static_cast<int>(a) ^ static_cast<int>(b)));
}

template <typename T>
constexpr auto operator|=(T &a, T b) ->
    typename std::enable_if<std::is_enum<T>::value, T>::type {
  return reinterpret_cast<T &>(
      (reinterpret_cast<int &>(a) |= static_cast<int>(b)));
}

template <typename T>
constexpr auto operator&=(T &a, T b) ->
    typename std::enable_if<std::is_enum<T>::value, T>::type {
  return reinterpret_cast<T &>(
      (reinterpret_cast<int &>(a) &= static_cast<int>(b)));
}

template <typename T>
constexpr auto operator^=(T &a, T b) ->
    typename std::enable_if<std::is_enum<T>::value, T>::type {
  return reinterpret_cast<T &>(
      (reinterpret_cast<int &>(a) ^= static_cast<int>(b)));
}
