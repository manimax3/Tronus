#include "StreamBuffer.h"
#include <sstream>

tr::StreamBuffer::StreamBuffer(StreamBuffer *target)
    : mTarget(target)
{
}

tr::StreamBuffer::~StreamBuffer()
{
    if (mTarget)
        delete mTarget;
}

tr::FileBuffer::FileBuffer(const std::string &path, int32 flags)
{
    try {
        Open(path, flags);
    } catch (FileOpenError e) {
        throw;
    }
}

tr::FileBuffer::~FileBuffer() { Close(); }

void tr::FileBuffer::Open(const std::string &path, int32 flags)
{
    if (mHandle)
        throw FileOpenError();

    std::stringstream ss;
    if (flags & WRITING) {
        ss << 'w';
        if (flags & READING)
            ss << '+';
        if (flags & SAFE)
            ss << 'x';
    } else if (flags & APPEND) {
        ss << 'a';
        if (flags & READING)
            ss << '+';
    } else if (flags & READING)
        ss << 'r';

    if (flags & BINARY)
        ss << 'b';

    FILE *file = fopen(path.c_str(), ss.str().c_str());

    if (!file)
        throw FileOpenError();

    mHandle = reinterpret_cast<std::uintptr_t>(file);
}

void tr::FileBuffer::Close()
{
    if (mHandle) {
        FILE *file = reinterpret_cast<FILE *>(mHandle);
        fclose(file);
    }
}

std::streamsize tr::FileBuffer::Write(
    const char *src, std::streamsize n) noexcept
{
    FILE *      file    = reinterpret_cast<FILE *>(mHandle);
    std::size_t written = fwrite(src, sizeof(char), n, file);

    return written;
}

std::streamsize tr::FileBuffer::Read(char *dest, std::streamsize n) noexcept
{
    FILE *      file = reinterpret_cast<FILE *>(mHandle);
    std::size_t read = fread(dest, sizeof(char), n, file);

    return read;
}

bool tr::FileBuffer::flush() noexcept
{
    FILE *file = reinterpret_cast<FILE *>(mHandle);
    return !fflush(file);
}
