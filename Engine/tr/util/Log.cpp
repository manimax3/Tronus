#include "Log.h"

tr::Log::Log()
    : mLevel(LogLevel::INFO)
{
    mFileStream.Push<FileBuffer>("tronus_log.txt", FileBuffer::WRITING);
}

tr::Log &tr::Log::operator<<(const char *str)
{
    const std::size_t length = strlen(str);
    mFileStream.Write(str, length);
    return *this;
}

tr::Log &tr::Log::operator<<(const LogLevel level)
{
    mLevel = level;
    return *this;
}

tr::Log &tr::Log::log(const std::string &out, LogLevel level)
{
    return *this << level << out;
}
