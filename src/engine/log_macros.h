
#pragma once

#define log_trace(...)                                                         \
  if (LogLevel::TRACE >= static_cast<LogLevel>(LOG_LEVEL))                     \
  log_me(LogLevel::TRACE, __FILE__, __LINE__, __VA_ARGS__)

#define log_info(...)                                                          \
  if (LogLevel::INFO >= static_cast<LogLevel>(LOG_LEVEL))                      \
  log_me(LogLevel::INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)                                                          \
  if (LogLevel::WARN >= static_cast<LogLevel>(LOG_LEVEL))                      \
  log_me(LogLevel::WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...)                                                         \
  if (LogLevel::ERROR >= static_cast<LogLevel>(LOG_LEVEL))                     \
    log_me(LogLevel::ERROR, __FILE__, __LINE__, __VA_ARGS__);                  \
  assert(false)

#define log_clean(level, ...)                                                  \
  if (level >= static_cast<LogLevel>(LOG_LEVEL))                               \
    log_me(level, "", -1, __VA_ARGS__);
