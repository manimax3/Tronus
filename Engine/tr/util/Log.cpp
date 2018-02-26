#include "Log.h"
#include "rang.hpp"
#include <mutex>

bool tr::Log::Initialize(Engine *engine)
{
    mFileStream.Push<FileBuffer>("tronus_log.txt", FileBuffer::WRITING);
    return Subsystem<Log>::Initialize(engine);
}

tr::Log &tr::Log::log(
    const std::string &out, LogLevel level, LogOptions options)
{
    using namespace LogOption;
    static std::mutex lck;

    std::string prefix;

    if (level == LogLevel::INFO)
        prefix = "[INFO] ";
    else if (level == LogLevel::WARNING)
        prefix = "[WARN] ";
    else if (level == LogLevel::ERROR)
        prefix = "[ERROR] ";

    prefix += out;

    if (options & AUTOENDL)
        prefix += '\n';

    lck.lock();

    if (mInitialized && (options & LOGFILE))
        mFileStream.Write(prefix.c_str(), prefix.length());

    if (options & CONSOLE) {
        if (level == LogLevel::WARNING)
            std::cout << rang::fg::yellow;
        else if (level == LogLevel::ERROR)
            std::cout << rang::fgB::red << rang::style::bold;
        std::cout << prefix;
        std::cout << rang::style::reset;
    }

    lck.unlock();

    return *this;
}
