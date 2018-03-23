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

void tr::World::StartWorld() { mEngine->sEventSystem->AddListener(this); }

void tr::World::StopWorld()
{
    mEngine->sEventSystem->RemoveListener(this);

    for (auto &go : mGameObjects) {
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
    for (auto &go : mGameObjects)
        go->HandleEvent(e);
}

void tr::World::RenderDebug()
{
    if (ImGui::Begin("World Debug")) {
        ImGui::Text("Count GameObjects: %li", mGameObjects.size());

        for (auto &go : mGameObjects) {
            if (ImGui::TreeNode(go->GetName().c_str())) {
                for (int i = 0; i < go->mComponents.size(); i++) {
                    ImGui::Text("%s", go->mComponents[i]->GetName().c_str());
                }
                ImGui::TreePop();
            }
        }
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
