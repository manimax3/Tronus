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

void tr::World::StartWorld()
{
    mEngine->sEventSystem->AddListener(this);
}

void tr::World::StopWorld()
{
    mEngine->sEventSystem->RemoveListener(this);

    for(auto go : mGameObjects) {
        go->LeaveWorld();
    }

    mGameObjects.clear();
}

std::vector<int> tr::World::SubscripeTo() const
{
    return { ENGINE_CHANNEL, RENDER_CHANNEL };
}

void tr::World::OnEvent(const Event &e, int channel)
{
}

void tr::World::RenderDebug()
{
    if (ImGui::Begin("World Debug")) {
        ImGui::Text("Count GameObjects: %li", mGameObjects.size());
    }
    ImGui::End();
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
