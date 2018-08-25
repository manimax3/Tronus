#include "Log.h"

#include "../core/Subsystem.h"
#include "../filesystem/Filesystem.h"
#include "../filesystem/Stream.h"
#include <bitset>
#include <tr.h>
#include <type_traits>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> tr::detail::CreateLoggerInternal()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%T] [%^%l%$] %v");
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        fs::GetExecutablePath() + "tronus_log.txt", true);
    file_sink->set_pattern("[%T] [%l] %v");

    std::vector<spdlog::sink_ptr> sinks{ console_sink, file_sink };

    auto log = std::make_shared<spdlog::logger>(
        "EngineLogger", std::begin(sinks), std::end(sinks));
    spdlog::register_logger(log);

#ifdef TR_DEBUG
    log->set_level(spdlog::level::debug);
#endif

    return log;
}

void tr::SetLogLevel(LogLevel level)
{
    switch (level) {
    case LogLevel::Debug:
        Log().set_level(spdlog::level::debug);
        break;
    case LogLevel::Info:
        Log().set_level(spdlog::level::info);
        break;
    case LogLevel::Warn:
        Log().set_level(spdlog::level::warn);
        break;
    case LogLevel::Error:
        Log().set_level(spdlog::level::err);
        break;
    case LogLevel::Critical:
        Log().set_level(spdlog::level::critical);
        break;
    }
}
