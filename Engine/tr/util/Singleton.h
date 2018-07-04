#pragma once
#include <tr.h>

#include <type_traits>

namespace tr {
template<typename T>
class Singleton {
protected:
    Singleton() noexcept = default;

public:
    virtual ~Singleton() = default;

    static T &Get() noexcept
    {
        static T t;
        return t;
    }
};
}
