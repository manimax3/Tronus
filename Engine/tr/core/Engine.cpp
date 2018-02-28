#include "Engine.h"
#include "../event/CommonEvents.h"
#include "../event/EventSystem.h"
#include "../graphics/GraphicsHandler.h"
#include "../util/Log.h"
#include "profile/Profiler.h"
#include "util/Timer.h"
#include <iostream>

using namespace tr;

Engine::Engine()
{
    AddSystem<Log>();
    AddSystem<JobHandler>();
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
    for (auto &subsystem : mSubsystems)
        subsystem.second->Initialize(this);

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
            std::cout << mUPS << '\n';
            mUPS = 0;
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
    for (auto &subsystem : mSubsystems)
        subsystem.second->Shutdown();
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
    if (e.Identifier == INPUT_ID) {
        const auto &ie = static_cast<const InputEvent &>(e);
        Logger().log(std::string("Pressed key: "));
    }
}
