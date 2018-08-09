#include "Engine.h"
#include "../event/CommonEvents.h"
#include "../filesystem/ResourceManager.h"
#include "../gameobject/Game.h"
#include "../gameobject/World.h"
#include "../graphics/DebugWindow.h"
#include "../graphics/GraphicsHandler.h"
#include "../profile/Profiler.h"
#include "../util/Keys.h"
#include "JobHandler.h"
#include "util/Timer.h"
#include <iostream>

using namespace tr;

Engine::Engine()
{
    // TODO: Why arent thos unique_ptr ?
    sJobHandler      = new JobHandler;
    sResourceManager = new ResourceManager;
    sProfiler        = new Profiler;
    sGraphicsHandler = new GraphicsHandler;
}

Engine::~Engine()
{
    delete mWorld;

    delete sJobHandler;
    delete sResourceManager;
    delete sProfiler;
    delete sGraphicsHandler;

    delete mDebugWindow;
}

void Engine::Start(Game *game)
{
    EASY_MAIN_THREAD;
    EASY_PROFILER_ENABLE;

    Log().warn("Tronus Engine Starting...");

    if (!(mGame = game)) {
        Log().error("No game instance provided aborting...");
        return;
    }

    Log().info("Starting Subsystem Initializatiion...");
    sJobHandler->Initialize(this);
    sResourceManager->Initialize(this);
    sProfiler->Initialize(this);
    sGraphicsHandler->Initialize(this);

    if (mGame->EngineCreateWindow)
        sGraphicsHandler->CreateWindow(CreateWindowInfo());

    mWorld = new World(*this);

    Log().info("Starting Subsystem Post Initialization...");
    sJobHandler->PostInit();
    sResourceManager->PostInit();
    sProfiler->PostInit();
    sGraphicsHandler->PostInit();

    sGraphicsHandler->WindowChanged.connect(
        [&](const WindowEvent &e) { this->OnEvent(e); });


    mGame->PreWorldLoad(*mWorld);
    mWorld->Load();
    mGame->PreWorldStart(*mWorld);
    mWorld->Start();

    mRunning = true;

    mDebugWindow = new DebugWindow(*this);

    // Start the tick loop

    Timer       timer, sleep_timer;
    const float msPerTick   = 1000.f / 60.f;
    float       updateTimer = 0;
    float       tickTimer   = 0;

    GraphicsHandler *const gfx = sGraphicsHandler;

    while (mRunning) {

        sleep_timer.Reset();

        if ((timer.GetElapsed() - tickTimer) >= 1000.f) {
            mLastUps = mUPS;
            mUPS     = 0;

            mLastFps = mFPS;
            mFPS     = 0;

            tickTimer += 1000.f;
        }

        if ((timer.GetElapsed() - updateTimer) >= msPerTick) {
            Tick();
            updateTimer += msPerTick;
            mUPS++;
        }

        mFPS++;
        gfx->Render();
    }
}

void Engine::Stop()
{
    mGame->OnWorldShutdown();

    /* mWorld->StopWorld(); */

    mRunning = false;
    sProfiler->Shutdown();
    sGraphicsHandler->Shutdown();
    sResourceManager->Shutdown();
    sJobHandler->Shutdown();
    Log().info("All subsystems have been stoppend");
}

void Engine::Tick()
{
    EASY_FUNCTION(profiler::colors::Red);

    sJobHandler->Tick();
    sResourceManager->Tick();
    sProfiler->Tick();
    sGraphicsHandler->Tick();

    mWorld->Update();
}

void Engine::OnEvent(const WindowEvent &e)
{
    if (e.type == WindowEvent::CLOSED) {
        this->Stop();
    }
}
