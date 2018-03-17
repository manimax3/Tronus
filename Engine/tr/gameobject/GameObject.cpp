#include "GameObject.h"
#include "World.h"
#include "../core/Engine.h"
#include "../event/EventSystem.h"

tr::GameObject::GameObject(World *w, const std::string &name, Mat4 transform)
    : mName(name)
{
    Context.Wrld       = w;
    Context.Eng        = w->mEngine;
    Context.ResManager = Context.Eng->sResourceManager;
    Context.EvtHandler = Context.Eng->sEventSystem;

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
