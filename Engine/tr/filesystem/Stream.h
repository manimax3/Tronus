#pragma once

#include "StreamBuffer.h"
#include <istream>
#include <tr.h>
#include <type_traits>

namespace tr {

struct StreamReadError : public std::exception {
};
struct StreamWriteError : public std::exception {
};
struct StreamBufferPushError : public std::exception {
};

class Instream {
public:
    explicit Instream() = default;
    virtual ~Instream()
    {
        if (mBuffer)
            delete mBuffer;
    };

    inline void Flush() {
        mBuffer->flush();
    }

    inline std::streamsize Read(char *dest, std::streamsize count) noexcept
    {
        return mBuffer->Read(dest, count);
    }

    template<typename T, typename = std::enable_if_t<std::is_pod_v<T>>>
    Instream &operator>>(T &&t)
    {
        std::size_t     size        = sizeof(T);
        std::streamsize amount_read = Read(reinterpret_cast<char *>(&t), size);

        if (size != amount_read)
            throw StreamReadError();

        return *this;
    }

    template<typename T, typename... Args>
    std::enable_if_t<std::is_base_of_v<StreamBuffer, T>> Push(Args &&... args)
    {
        if constexpr (std::is_constructible<T, StreamBuffer *, Args...>::value)
            mBuffer = new T(mBuffer, std::forward<Args>(args)...);
        else if constexpr (std::is_constructible<T, Args...>::value) {
            if (mBuffer)
                delete mBuffer;
            mBuffer = new T(std::forward<Args>(args)...);
        } else
            throw StreamBufferPushError();
    }

private:
    StreamBuffer *mBuffer = nullptr;
};

class Outstream {
public:
    explicit Outstream() = default;
    virtual ~Outstream()
    {
        mBuffer->flush();
        if (mBuffer)
            delete mBuffer;
    };

    inline std::streamsize Write(
        const char *src, std::streamsize count) noexcept
    {
        return mBuffer->Write(src, count);
    }

    inline void Flush() {
        mBuffer->flush();
    }

    template<typename T, typename = std::enable_if_t<std::is_pod_v<T>>>
    Outstream &operator<<(T &&t)
    {
        std::size_t     size        = sizeof(T);
        std::streamsize amount_read = Write(reinterpret_cast<char *>(&t), size);

        if (size != amount_read)
            throw StreamWriteError();

        return *this;
    }

    template<typename T, typename... Args>
    std::enable_if_t<std::is_base_of_v<StreamBuffer, T>> Push(Args &&... args)
    {
        if constexpr (std::is_constructible<T, StreamBuffer *, Args...>::value)
            mBuffer = new T(mBuffer, std::forward<Args>(args)...);
        else if constexpr (std::is_constructible<T, Args...>::value) {
            if (mBuffer)
                delete mBuffer;
            mBuffer = new T(std::forward<Args>(args)...);
        } else
            throw StreamBufferPushError();
    }

private:
    StreamBuffer *mBuffer = nullptr;
};
}
