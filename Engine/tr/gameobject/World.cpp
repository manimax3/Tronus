#include "World.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../event/EventSystem.h"
#include "../profile/Profiler.h"

#include "imgui.h"

tr::World::World(Engine *engine)
    : mEngine(engine)
{
}

void tr::World::StartWorld() {
    if(mEngine) {
        mEngine->sEventSystem->AddListener(this);
    }
}

std::vector<int> tr::World::SubscripeTo() const
{
    return { ENGINE_CHANNEL, RENDER_CHANNEL };
}

void tr::World::OnEvent(const Event &e, int channel)
{
    if (channel == ENGINE_CHANNEL) {
    } else if (channel == RENDER_CHANNEL) {
        if (e.Identifier == event::RENDER_DEBUG) {
            DispatchDebugGuiEvent();
        }
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
    EASY_BLOCK("World Tick");

    for (auto &go : mGameObjects) {
        if (go->mTickable) {
            go->OnTick();
        }
    }
}
