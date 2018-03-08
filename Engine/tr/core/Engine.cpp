#include "Engine.h"
#include "../event/CommonEvents.h"
#include "../event/EventSystem.h"
#include "../filesystem/ResourceManager.h"
#include "../graphics/GraphicsHandler.h"
#include "../util/Keys.h"
#include "../util/Log.h"
#include "profile/Profiler.h"
#include "util/Timer.h"
#include <iostream>

using namespace tr;

Engine::Engine()
{
    AddSystem<Log>();
    AddSystem<JobHandler>();
    AddSystem<ResourceManager>();
    AddSystem<Profiler>();
    AddSystem<EventSystem>();
    AddSystem<GraphicsHandler>();
}

void Engine::Start()
{
    EASY_MAIN_THREAD;
    EASY_PROFILER_ENABLE;

    mLog = GetSystem<Log>();
    Logger().log("Tronus Engine Starting...", LogLevel::WARNING);

    // Init all the subsystems
    for (auto &subsystem : mSubsystems) {
        Logger().log("Starting "s + subsystem.second->GetName());
        if (!subsystem.second->Initialize(this))
            Logger().log("Problem during starting of: "s
                             + subsystem.second->GetName(),
                         LogLevel::ERROR);
    }

    for (auto &subsystem : mSubsystems)
        subsystem.second->PostInit();

    mRunning = true;

    GetSystem<EventSystem>()->AddListener(this);

    // Start the tick loop

    Timer       timer, sleep_timer;
    const float msPerTick   = 1000.f / 60.f;
    float       updateTimer = 0;
    float       tickTimer   = 0;

    GraphicsHandler *const gfx = GetSystem<GraphicsHandler>();

    while (mRunning) {

        sleep_timer.Reset();

        if ((timer.GetElapsed() - tickTimer) >= 1000.f) {
            mLastUps = mUPS;
            mUPS     = 0;
            tickTimer += 1000.f;
        }

        if ((timer.GetElapsed() - updateTimer) >= msPerTick) {
            Tick();
            updateTimer += msPerTick;
            mUPS++;
        }

        gfx->Render();
    }
}

void Engine::Stop()
{
    mRunning = false;
    for (auto &subsystem : mSubsystems) {
        subsystem.second->Shutdown();
        Logger().log("Stopped "s + subsystem.second->GetName());
    }
}

void Engine::Tick()
{
    EASY_FUNCTION();

    for (auto &subsystem : mSubsystems)
        subsystem.second->Tick();
}

std::vector<int> tr::Engine::SubscripeTo() const { return { ENGINE_CHANNEL }; }

void Engine::OnEvent(const Event &e, int channel)
{
    if (e.Identifier == event::INPUT) {
        const auto &ie = static_cast<const InputEvent &>(e);
        if (ie.type == InputEvent::Keyboard && ie.action == InputEvent::PRESS
            && ie.Key == KEY_F3)
            Logger().log("UPS: "s + std::to_string(mLastUps));
        else if (ie.type == InputEvent::Keyboard
                 && ie.action == InputEvent::PRESS && ie.Key == KEY_F4) {
            auto *rm = GetSystem<ResourceManager>();
            rm->LoadResource("test.json");
            rm->LoadResource("test_shader.json");

            auto *test  = rm->GetRes<StringResource>("test.json");
            auto *test2 = rm->GetRes<StringResource>("test2.json");

            if (test)
                Logger().log(test->data);
            if (test2)
                Logger().log(test2->data);
        }
    } else if (e.Identifier == event::WINDOW) {
        const auto &we = static_cast<const WindowEvent &>(e);
        if (we.type == WindowEvent::CLOSED) {
            this->Stop();
        }
    }
}
