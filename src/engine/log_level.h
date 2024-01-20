#pragma once

#include "globals.h" // where LOG_LEVEL is located

enum struct LogLevel {
  ALL = 0, // Display all logs
  TRACE,   // Trace logging, intended for internal use only
  DEBUG, // Debug logging, used for internal debugging, it should be disabled on
         // release builds
  INFO,  // Info logging, used for program execution info
  WARN,  // Warning logging, used on recoverable failures
  ERROR, // Error logging, used on unrecoverable failures
  FATAL, // Fatal logging, used to abort program: exit(EXIT_FAILURE)
  NONE   // Disable logging
};
