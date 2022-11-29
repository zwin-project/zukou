#include "logger.h"

namespace zukou {

void
print_log(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
}

}  // namespace zukou
