#include "World.h"
#include "../core/Engine.h"
#include "../profile/Profiler.h"

tr::World::World(Engine &engine)
    : mEngine(engine)
{
}

void tr::World::Update()
{
    EASY_BLOCK("World Update");

    for (const auto &[name, ptr] : mWorldCapabilities) {
        ptr->OnUpdate();
    }

    for (auto seg : mGameObjects.segment_traversal()) {
        for (auto &go : seg) {
            if (go.IsGameObjectTicking()) {
                go.Update();
            }
        }
    }
}

void tr::World::Start()
{
    for (const auto &[name, ptr] : mWorldCapabilities) {
        ptr->OnStart();
    }
}

void tr::World::Load()
{
    for (const auto &[name, ptr] : mWorldCapabilities) {
        ptr->OnLoad();
    }
}

void tr::World::SetupInternal(GameObject *ptr)
{
    assert(ptr);
    ptr->EnterWorld(*this);
}
