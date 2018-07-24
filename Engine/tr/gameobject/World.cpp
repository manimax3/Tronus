#include "World.h"
#include "../core/Engine.h"

tr::World::World(Engine &engine)
    : mEngine(engine)
{
}

void tr::World::Update()
{
    for (auto seg : mGameObjects.segment_traversal()) {
        for (auto &go : seg) {
            if (go.IsGameObjectTicking()) {
                go.Update();
            }
        }
    }
}

void tr::World::Start() {}

void tr::World::Load() {}

void tr::World::SetupInternal(GameObject *ptr)
{
    assert(ptr);
    ptr->EnterWorld(*this);
}
