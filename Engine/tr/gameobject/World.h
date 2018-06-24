#pragma once
#include <tr.h>

#include "../core/Subsystem.h"
#include "../event/EventListener.h"
#include "../util/MakeID.h"
#include "../util/TemplateMeta.h"
#include "Component.h"
#include "GameObject.h"

#include <memory>
#include <vector>

namespace tr {

class World;

class BaseSystem : public EventListener {
public:
    friend class World;

    explicit BaseSystem(World &world) noexcept
        : mWorld(world){};
    virtual ~BaseSystem() = default;

    virtual void OnUpdate(){};

    virtual void             OnEvent(const Event &, int channel) override{};
    virtual std::vector<int> SubscripeTo() const override { return {}; };

protected:
    virtual void HandleGameObjectAdd(GameObjectHandle go)             = 0;
    virtual void HandleGameObjectRemove(GameObjectHandle go)          = 0;
    virtual void HandleGameObjectComponentUpdate(GameObjectHandle go) = 0;

    World &mWorld;
};

class World {
public:
    explicit World(BaseSubsystem *s = nullptr) noexcept;

    void Update();
    void DestroyGameObject(GameObjectHandle go);
    void UpdateGameObject(GameObjectHandle go);

    template<typename... C>
    GameObjectHandle CreateGameObject()
    {

        static_assert((std::is_default_constructible_v<C> && ...),
                      "CreateGameObject<C...> Only works with default "
                      "constructible components");

        uint id;
        mGameObjectsGenerator.CreateID(id);

        GameObject go(id);
        (go.AttachComponent<C>(), ...);

        mGameObjects[id] = std::move(go);

        for (auto &system : mSystems)
            system->HandleGameObjectAdd(id);

        return id;
    }

    template<typename T>
    void AttachSystem()
    {
        static_assert(std::is_base_of_v<BaseSystem, T>);

        mSystems.emplace_back(new T(*this));
        SetupSystemInternal(mSystems.back().get());
        for (auto &[handle, go] : mGameObjects) {
            mSystems.back()->HandleGameObjectAdd(handle);
        }
    };

    template<typename... Comps>
    std::tuple<Comps *...> GetComponents(GameObjectHandle handle)
    {
        static_assert(mp::is_unique_v<Comps...>);

        auto &go = mGameObjects[handle];
        return std::make_tuple(go.GetComponent<Comps>()...);
    }

    template<typename Comp>
    Comp *GetComponent(GameObjectHandle handle)
    {
        return std::get<0>(GetComponents<Comp>(handle));
    }

    template<typename... C>
    bool MatchesComponents(GameObjectHandle handle)
    {
        static_assert(mp::is_unique_v<C...>);

        auto &     go = mGameObjects[handle];
        const bool r  = (go.GetComponent<C>() && ...);
        return r;
    }

    // Returns GameObject::INVALID if the gameobject wasnt found
    GameObject& GetGameObject(GameObjectHandle handle);

    Engine *mEngine = nullptr;

private:
    void SetupSystemInternal(BaseSystem *system);

    std::unordered_map<GameObjectHandle, GameObject> mGameObjects;
    std::vector<std::unique_ptr<BaseSystem>>         mSystems;

    MakeID mGameObjectsGenerator;
};
}
