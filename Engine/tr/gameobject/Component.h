#pragma once
#include <tr.h>

#include <memory>

namespace tr {

using ComponentTypeID = uint;

namespace detail::ecs {
    extern ComponentTypeID component_type_counter;
}

template<typename T>
inline const auto CompIDResolver = ++detail::ecs::component_type_counter;

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
