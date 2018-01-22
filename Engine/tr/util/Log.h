#pragma once

#include "../core/Subsystem.h"
#include "../filesystem/Stream.h"
#include <tr.h>

namespace tr {

enum class LogLevel { ERROR, WARNING, INFO };

class Log : public Subsystem<Log> {
public:
    explicit Log();

    Log &log(const std::string &out, LogLevel level);

    Log &operator<<(const LogLevel level);
    Log &operator<<(const char *str);

    inline Log &operator<<(const std::string &out)
    {
        return *this << out.c_str();
    };
    inline Log &operator<<(int8 message)
    {
        return *this << std::to_string(message);
    };
    inline Log &operator<<(int16 message)
    {
        return *this << std::to_string(message);
    };
    inline Log &operator<<(int32 message)
    {
        return *this << std::to_string(message);
    };
    inline Log &operator<<(int64 message)
    {
        return *this << std::to_string(message);
    };
    inline Log &operator<<(uint8 message)
    {
        return *this << std::to_string(message);
    };
    inline Log &operator<<(uint16 message)
    {
        return *this << std::to_string(message);
    };
    inline Log &operator<<(uint32 message)
    {
        return *this << std::to_string(message);
    };
    inline Log &operator<<(uint64 message)
    {
        return *this << std::to_string(message);
    };
    inline Log &operator<<(float message)
    {
        return *this << std::to_string(message);
    };
    inline Log &operator<<(bool message)
    {
        return *this << (message ? "True" : "False");
    };

private:
    Outstream mFileStream;
    LogLevel  mLevel;
};
}
