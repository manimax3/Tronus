#pragma once
#include <tr.h>

#include "../util/TemplateMeta.h"
#include "Component.h"
#include "GameObject.h"
#include "World.h"

#include <functional>

namespace tr {

template<typename... Components>
class System : public BaseSystem {

    static_assert(mp::is_unique_v<Components...>);

public:
    using ComponentTuple = std::tuple<Components *...>;

    explicit System(World &world) noexcept
        : BaseSystem(world)
    {
    }

    ~System() = default;

    virtual void OnGameObjectAdd(GameObjectHandle handle){};
    virtual void OnGameObjectRemove(GameObjectHandle handle){};

    void HandleGameObjectAdd(GameObjectHandle handle) override
    {
        if (GameObjectKnown(handle))
            return;

        if (!mWorld.MatchesComponents<Components...>(handle))
            return;

        mRegisteredGameObjects[handle]
            = std::move(mWorld.GetComponents<Components...>(handle));

        OnGameObjectAdd(handle);
    }

    void HandleGameObjectRemove(GameObjectHandle handle) override
    {
        if (const auto i = mRegisteredGameObjects.find(handle);
            i != std::end(mRegisteredGameObjects))
            mRegisteredGameObjects.erase(i);

        OnGameObjectRemove(handle);
    }

    void HandleGameObjectComponentUpdate(GameObjectHandle handle) override
    {
        // We just remove it and add it again
        // the add method shoudl only actually do this
        // if none of the components is nullptr
        HandleGameObjectRemove(handle);
        HandleGameObjectAdd(handle);
    }

    void ForEachTuple(const std::function<bool(ComponentTuple &)> &function)
    {
        for (auto &[handle, tuple] : mRegisteredGameObjects) {
            if (!function(tuple))
                break;
        }
    }

protected:
    bool GameObjectKnown(GameObjectHandle handle)
    {
        return mRegisteredGameObjects.find(handle)
            != std::end(mRegisteredGameObjects);
    }

protected:
    using GoCompMap = std::unordered_map<GameObjectHandle, ComponentTuple>;
    GoCompMap mRegisteredGameObjects;
};
}
