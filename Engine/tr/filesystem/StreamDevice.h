#pragma once

#include <fstream>
#include <tr.h>

namespace tr {
class StreamDevice : public virtual std::streambuf {
public:
    explicit StreamDevice(std::streambuf *target)
        : mTargetBuffer(target)
    {
    }

    virtual ~StreamDevice()
    {
        if (mTargetBuffer)
            delete mTargetBuffer;
    }

    StreamDevice *SetTargetBuffer(std::streambuf *target)
    {
        mTargetBuffer = target;
        return this;
    }

private:
    std::streambuf *mTargetBuffer;
};

class AdapterDevice {
public:
    virtual ~AdapterDevice()      = default;
    virtual std::streambuf *Get() = 0;
};

class FileDevice : public AdapterDevice {
public:
    using Mode                    = std::ios_base::openmode;
    static constexpr Mode WRITING = std::ios::out;
    static constexpr Mode READING = std::ios::in;
    static constexpr Mode APPEND  = std::ios::app;
    static constexpr Mode DELETE  = std::ios::trunc;
    static constexpr Mode BINARY  = std::ios::binary;

public:
    explicit FileDevice(const std::string &path, std::ios_base::openmode mode);

    std::streambuf *Get() override;

private:
    std::filebuf *mBuffer;
};
}
