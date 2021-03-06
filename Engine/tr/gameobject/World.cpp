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
            // GOs check for themselve if they need to tick
            go.Update();
        }
    }
}

void tr::World::Start()
{
    EASY_BLOCK("World Start");
    for (const auto &[name, ptr] : mWorldCapabilities) {
        ptr->OnStart();
    }
}

void tr::World::Load()
{
    EASY_BLOCK("World Load");
    for (const auto &[name, ptr] : mWorldCapabilities) {
        ptr->OnLoad();
    }
}

void tr::World::SetupInternal(GameObject *ptr)
{
    assert(ptr);
    ptr->EnterWorld(*this);
}
