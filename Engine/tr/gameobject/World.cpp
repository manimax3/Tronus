#include "World.h"

#include "../core/Engine.h"
#include "Sprite2DComponent.h"

tr::GameObject tr::GameObject::INVALID = GameObject();

tr::World::World(BaseSubsystem *s) noexcept
    : mEngine(s ? &s->GetEngine() : nullptr)
{
    uint i;
    mGameObjectsGenerator.CreateID(i);
    AttachSystem<SpriteRenderSystem>();
};

void tr::World::Update()
{
    for (auto &system : mSystems) {
        system->OnUpdate();
    }
}

void tr::World::DestroyGameObject(GameObjectHandle handle)
{

    for (auto &system : mSystems) {
        system->HandleGameObjectRemove(handle);
    }

    mGameObjects.erase(handle);
    mGameObjectsGenerator.DestroyID(handle);
}

void tr::World::UpdateGameObject(GameObjectHandle handle)
{
    for (auto &system : mSystems) {
        system->HandleGameObjectComponentUpdate(handle);
    }
}

void tr::World::SetupSystemInternal(BaseSystem *s)
{
    if (!mEngine)
        return;
}

tr::GameObject &tr::World::GetGameObject(GameObjectHandle handle)
{
    if (const auto &go = mGameObjects.find(handle);
        go != std::end(mGameObjects)) {
        return std::get<1>(*go);
    }

    if (mEngine)
        mEngine->sLog->log("Could not find gameobject with handle: "s
                               + std::to_string(handle),
                           LogLevel::ERROR);

    return GameObject::INVALID;
}
