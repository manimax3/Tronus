#include "World.h"
#include "../core/Engine.h"
#include "../event/EventSystem.h"

tr::World::World(Engine *engine)
    : mEngine(engine)
{
}

void tr::World::StartWorld() {}

std::vector<int> tr::World::SubscripeTo() const
{
    return { ENGINE_CHANNEL, RENDER_CHANNEL };
}

void tr::World::OnEvent(const Event &e, int channel)
{
    if (channel == ENGINE_CHANNEL) {
    } else if (channel == RENDER_CHANNEL) {
    }
}

void tr::World::DispatchDebugGuiEvent()
{
    for (auto &go : mGameObjects) {
        go->OnDebugGui();
    }
}

void tr::World::DispatchTick()
{
    for (auto &go : mGameObjects) {
        if (go->mTickable) {
            go->OnTick();
        }
    }
}
