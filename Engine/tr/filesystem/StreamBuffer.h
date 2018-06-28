#pragma once

#include <iostream>
#include <tr.h>

namespace tr {
class StreamBuffer {
public:
    explicit StreamBuffer(StreamBuffer *target = nullptr);
    virtual ~StreamBuffer();

    virtual std::streamsize Write(const char *src, std::streamsize n) noexcept = 0;
    virtual std::streamsize Read(char *dest, std::streamsize n) noexcept   = 0;
    virtual bool            flush() noexcept                              = 0;

private:
    StreamBuffer *mTarget = nullptr;
};

struct FileOpenError : public std::exception {
};

class FileBuffer : public StreamBuffer {
    public:
        enum {
            WRITING = 0b1,
            READING = 0b10,
            APPEND  = 0b100,
            BINARY  = 0b1000,
            SAFE    = 0b10000
        };

    public:
        explicit FileBuffer(const std::string &path, int32 flags);
        explicit FileBuffer() = default;
        ~FileBuffer() override;

        void Open(const std::string &path, int32 flags);
        void Close();

        std::streamsize Write(const char *src, std::streamsize n) noexcept override;
        std::streamsize Read(char *dest, std::streamsize n) noexcept override;
        bool flush() noexcept override;

    private:
        std::uintptr_t mHandle = 0;
};
}
