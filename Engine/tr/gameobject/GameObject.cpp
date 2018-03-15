#include "GameObject.h"

tr::GameObject::GameObject(World *w, const std::string &name, Mat4 transform)
    : mWorld(w)
    , mName(name)
{
    cRootComponent = CreateComponent<SceneComponent>("RootComponent");
}

void tr::GameObject::EnterWorld()
{
    cRootComponent->WorldEnter();
    OnWorldEnter();
}

void tr::GameObject::LeaveWorld()
{
    cRootComponent->WorldLeave();
    OnWorldLeave();
}

void tr::GameObject::TickGameObject(double delta)
{
    mLastDelta = delta;

    if (mTickable) {
        OnTick();
        for (auto &c : mComponents) {
            if (c->mTickable)
                c->TickComponent();
        }
    }
}
