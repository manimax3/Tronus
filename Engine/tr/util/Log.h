#pragma once

#include "enum.h"
#include "spdlog/logger.h"

#include <string>

namespace tr {
using namespace std::literals::string_literals;

BETTER_ENUM(LogLevel, int, Debug, Info, Warn, Error, Critical);

namespace detail {
    std::shared_ptr<spdlog::logger> CreateLoggerInternal();
}

void SetLogLevel(LogLevel level);

inline auto &Log()
{
    static auto Logger = detail::CreateLoggerInternal();
    return *Logger;
}
}
