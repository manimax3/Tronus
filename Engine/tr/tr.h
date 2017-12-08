#pragma once

#include <cmath>
#include <cstddef>
#include <cstring>
#include <stdint.h>

#ifdef TR_PLATFORM_WINDOWS
#ifdef TR_CORE_DLL
#define TR_API __declspec(dllexport)
#else
#define TR_API __declspec(dllimport)
#endif
#else
#define TR_API
#endif

namespace tr {
typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef uint8  byte;
typedef uint32 uint;
}
