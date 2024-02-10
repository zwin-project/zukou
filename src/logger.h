#pragma once

#include <stdarg.h>
#include <stdio.h>

namespace zukou {

void print_log(const char *format, ...)
    __attribute__((__format__(printf, 1, 2)));

}  // namespace zukou

#define LOG_DEBUG(format, ...) \
  zukou::print_log("[zukou] DEBUG: " format "\n", ##__VA_ARGS__)

#define LOG_INFO(format, ...) \
  zukou::print_log("[zukou]  INFO: " format "\n", ##__VA_ARGS__)

#define LOG_WARN(format, ...) \
  zukou::print_log("[zukou]  WARN: " format "\n", ##__VA_ARGS__)

#define LOG_ERROR(format, ...) \
  zukou::print_log("[zukou] ERROR: " format "\n", ##__VA_ARGS__)

#define LOG_FATAL(format, ...) \
  zukou::print_log("[zukou] FATAL: " format "\n", ##__VA_ARGS__)
