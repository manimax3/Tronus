#pragma once

#include <cmath>
#include <cstddef>
#include <cstring>
#include <cstdint>

#ifdef TR_PLATFORM_WINDOWS
#ifdef TR_CORE_DLL
#define TR_API __declspec(dllexport)
#else
#define TR_API __declspec(dllimport)
#endif
#else
#define TR_API
#endif

#ifndef NDEBUG
#define TR_DEBUG
#endif

namespace tr {
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using byte = uint8;
using uint = uint32;
}
