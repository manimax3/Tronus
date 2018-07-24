#include "GameObject.h"

#include "World.h"

#include <assert.h>

tr::GameObjectComponent &tr::GameObject::GetRootComponent()
{
    static SceneComponent s;
    return HasRootComponent() ? *RootComponent : s;
}

void tr::GameObject::Update()
{
    for (auto &[k, v] : mComponents) {
        if (v->IsComponentTicking())
            v->UpdateComponent();
    }
}

void tr::GameObject::EnterWorld(World &world)
{
    assert(!mWorld);
    mWorld = &world;
    OnWorldEnter();
}

tr::World &tr::GameObject::GetWorld()
{
    if (!mWorld) {
        throw std::runtime_error("GameObject doesnt belong to a world.");
    }

    return *mWorld;
}
