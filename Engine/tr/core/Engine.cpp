#include "Engine.h"
#include "util/Timer.h"
#include <iostream>

using namespace tr;

Engine::Engine()
{
    AddSystem<JobHandler>();
    AddSystem<Profiler>();
}

void Engine::Start()
{
    EASY_MAIN_THREAD;
    EASY_PROFILER_ENABLE;
    
    // Init all the subsystems
    for(auto& subsystem : mSubsystems)
        subsystem.second->Initialize(this);
    
    mRunning = true;
    
    // Start the tick loop
    
    Timer timer;
    const float msPerTick = 1000.f / 60.f;
    float updateTimer = 0;
    float tickTimer = 0;
    
    while(mRunning)
    {
        
        if((timer.GetElapsed() - tickTimer) >= 1000.f)
        {
            std::cout << mUPS << '\n';
            mUPS = 0;
            tickTimer += 1000.f;
        }
        
        while((timer.GetElapsed() - updateTimer) >= msPerTick)
        {
            Tick();
            updateTimer += msPerTick;
            mUPS++;
        }
        
    }
    
}

void Engine::Stop()
{
    mRunning = false;
    for(auto& subsystem : mSubsystems)
        subsystem.second->Shutdown();
}

void Engine::Tick()
{
    EASY_FUNCTION();
    
    for(auto& subsystem : mSubsystems)
        subsystem.second->Tick();
}