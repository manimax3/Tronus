#include "Engine.h"
#include "../util/Log.h"
#include "util/Timer.h"
#include <iostream>
#include "../event/EventSystem.h"

using namespace tr;

Engine::Engine()
{
    AddSystem<Log>();
    AddSystem<JobHandler>();
    AddSystem<Profiler>();
    AddSystem<EventSystem>();
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

    // Start the tick loop

    Timer       timer, sleep_timer;
    const float msPerTick   = 1000.f / 60.f;
    float       updateTimer = 0;
    float       tickTimer   = 0;

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

        if (sleep_timer.GetElapsed() < msPerTick) {
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(
                static_cast<uint64>(msPerTick - sleep_timer.GetElapsed())));
        }
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
