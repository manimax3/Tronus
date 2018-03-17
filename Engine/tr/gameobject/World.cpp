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
}

void tr::World::StopWorld()
{
    for(auto go : mGameObjects) {
        go->LeaveWorld();
    }

    mGameObjects.clear();
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
