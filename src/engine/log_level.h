#pragma once

#include "globals.h" // where LOG_LEVEL is located

enum struct LogLevel {
  LOG_ALOG_ = 0,
  TRACE = 1,
  INFO = 2,
  WARN = 3,
  ERROR = 4,
  NOTHING = 99,
};
