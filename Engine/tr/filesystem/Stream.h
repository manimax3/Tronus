#pragma once

#include <istream>
#include <tr.h>
#include <type_traits>
#include "StreamDevice.h"

namespace tr {
class Instream : public std::istream {
public:
    explicit Instream(std::streambuf *in = nullptr)
        : std::istream(in)
    {
    }

    virtual ~Instream()
    {
        if (this->rdbuf())
            delete this->rdbuf();
    }
    template<typename Device, typename... Args>
    void Push(Args &&... args)
    {
        if constexpr (std::is_base_of_v<StreamDevice, Device>) {
            this->rdbuf(static_cast<std::streambuf *>(
                new Device(this->rdbuf(), std::forward<Args>(args)...)));
        } else if constexpr (std::is_base_of_v<AdapterDevice, Device>) {
            Device d(std::forward<Args>(args)...);
            this->rdbuf(d.Get());
        }
    }
};

class Outstream : public std::ostream {
public:
    explicit Outstream(std::streambuf *in = nullptr)
        : std::ostream(in)
    {
    }

    virtual ~Outstream()
    {
        if (this->rdbuf())
            delete this->rdbuf();
    }

    template<typename Device, typename... Args>
    void Push(Args &&... args)
    {
        if constexpr (std::is_base_of_v<StreamDevice, Device>) {
            this->rdbuf(static_cast<std::streambuf *>(
                new Device(this->rdbuf(), std::forward<Args>(args)...)));
        } else if constexpr (std::is_base_of_v<AdapterDevice, Device>) {
            Device d(std::forward<Args>(args)...);
            this->rdbuf(d.Get());
        }
    }
};
}
