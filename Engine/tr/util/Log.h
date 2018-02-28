#pragma once

#include "../core/Subsystem.h"
#include "../filesystem/Stream.h"
#include <bitset>
#include <tr.h>
#include <type_traits>

namespace tr {

using LogOptions = uint32;

namespace LogOption {
    constexpr auto CONSOLE  = 1 << 0;
    constexpr auto LOGFILE  = 1 << 1;
    constexpr auto AUTOENDL = 1 << 2;
}

enum class LogLevel { INFO, WARNING, ERROR };

class Log : public Subsystem<Log> {
public:
    static constexpr LogOptions DEFAULT
        = LogOption::CONSOLE | LogOption::LOGFILE | LogOption::AUTOENDL;

    bool Initialize(Engine *engine) override;

    template<typename T>
    Log &log(const T &t, LogLevel level = LogLevel::INFO,
        LogOptions options = DEFAULT)
    {
        if constexpr (std::is_convertible_v<std::decay_t<T>, std::string>)
            this->log(static_cast<std::string>(t), level);
        else
            this->log(std::to_string(t), level);

        return *this;
    }

    Log &log(const std::string &out, LogLevel level = LogLevel::INFO,
        LogOptions options = DEFAULT);

    // Use this only if no other choise
    static Log *STATIC_LOGGER;

private:
    Outstream mFileStream;
};
}
