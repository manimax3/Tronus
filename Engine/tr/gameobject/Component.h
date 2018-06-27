#pragma once
#include <tr.h>

#include "../util/ClassID.h"

#include <memory>

namespace tr {

    struct ClassID_ComponentTag {};

using ComponentTypeID = int64;

template<typename T>
inline const auto CompIDResolver = ClassID<T, ClassID_ComponentTag>;

template<typename T, typename Allocator = std::allocator<T>>
struct ComponentTag {

    template<typename... Args>
    static T *New(Args &&... args)
    {
        auto *comp = allocator.allocate(1);
        new (comp) T(std::forward<Args>(args)...);
        return comp;
    }

    static void Delete(T *comp)
    {
        comp->~T();
        allocator.deallocate(comp, 1);
    }

    static Allocator allocator;
};

template<typename T, typename Allocator>
Allocator ComponentTag<T, Allocator>::allocator;
}
