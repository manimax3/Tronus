#pragma once

#include "../core/Subsystem.h"
#include "../filesystem/Stream.h"
#include "../filesystem/Filesystem.h"
#include <bitset>
#include <string>
#include <tr.h>
#include <type_traits>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace tr {
using namespace std::literals::string_literals;

namespace detail {
    inline auto CreateLoggerInternal()
    {
        auto console_sink
            = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern("[%T] [%l] %v");
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            fs::GetExecutablePath() + "tronus_log.txt");
        file_sink->set_pattern("[%T] [%l] %v");

        std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };
        auto log = std::make_shared<spdlog::logger>("EngineLogger", std::begin(sinks), std::end(sinks));
        spdlog::register_logger(log);
        return log;
    }
}

inline auto &Log()
{
    static auto Logger = detail::CreateLoggerInternal();
    return *Logger;
}
}
