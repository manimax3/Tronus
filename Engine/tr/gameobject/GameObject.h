#pragma once
#include <tr.h>

#include "Component.h"

#include <assert.h>
#include <unordered_map>

namespace tr {

class GameObject {

    // Invariants:
    // 1. GameObjects "own" their components
    // 2. For every component their is a deleter
    // 3. Never more than one component of the same type

public:
    using Handle = uint32;

    static GameObject INVALID; 

    explicit GameObject(Handle handle = 0) noexcept
        : mHandle(handle)
    {
    }

    GameObject(const GameObject &) = delete;
    GameObject &operator=(const GameObject &) = delete;

    GameObject(GameObject &&other)
        : mHandle(other.mHandle)
        , mComponents(std::move(other.mComponents))
        , mComponentDeleters(std::move(other.mComponentDeleters))
    {
        other.mHandle = 0;
    }

    GameObject &operator=(GameObject &&other)
    {
        mHandle            = other.mHandle;
        other.mHandle      = 0;
        mComponents        = std::move(other.mComponents);
        mComponentDeleters = std::move(other.mComponentDeleters);
        return *this;
    }

    ~GameObject()
    {
        if (mHandle == 0)
            return;

        for (auto [id, ptr] : mComponents) {
            auto *delter = mComponentDeleters[id];
            delter(ptr);
        }
    }

    template<typename C>
    bool HasComponent() noexcept
    {
        assert(mHandle);
        return mComponents.find(CompIDResolver<C>) != std::end(mComponents);
    }

    template<typename C>
    C *GetComponent() noexcept
    {
        assert(mHandle);

        return static_cast<C *>(
            HasComponent<C>() ? mComponents[CompIDResolver<C>] : nullptr);
    }

    template<typename C, typename... Args>
    C *AttachComponent(Args &&... args)
    {
        assert(mHandle);

        if (HasComponent<C>())
            return GetComponent<C>();

        C *comp = ComponentTag<C>::New(std::forward<Args>(args)...);

        mComponents[CompIDResolver<C>] = comp;
        mComponentDeleters[CompIDResolver<C>]
            = [](void *c) { ComponentTag<C>::Delete(static_cast<C *>(c)); };

        return comp;
    }

    template<typename C>
    void RemoveComponent() noexcept
    {
        assert(mHandle);

        auto *comp = GetComponent<C>();

        if (!comp)
            return;

        // Delete the component
        mComponentDeleters[CompIDResolver<C>](comp);

        mComponentDeleters.erase(mComponentDeleters.find(CompIDResolver<C>));
        mComponents.erase(mComponents.find(CompIDResolver<C>));
    }

private:
    Handle mHandle;

    std::unordered_map<ComponentTypeID, void *>           mComponents;
    std::unordered_map<ComponentTypeID, void (*)(void *)> mComponentDeleters;
};

using GameObjectHandle = GameObject::Handle;
}
