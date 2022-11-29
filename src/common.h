#pragma once

#if defined(__GNUC__) && __GNUC__ >= 4
#define ZUKOU_EXPORT __attribute__((visibility("default")))
#else
#define ZUKOU_EXPORT
#endif
