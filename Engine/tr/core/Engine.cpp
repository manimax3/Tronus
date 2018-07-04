#include "Engine.h"
#include "../event/CommonEvents.h"
#include "../event/EventSystem.h"
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
    sLog             = new Log;
    sJobHandler      = new JobHandler;
    sResourceManager = new ResourceManager;
    sProfiler        = new Profiler;
    sEventSystem     = new EventSystem;
    sGraphicsHandler = new GraphicsHandler;
}

Engine::~Engine()
{
    delete mWorld;

    delete sLog;
    delete sJobHandler;
    delete sResourceManager;
    delete sProfiler;
    delete sEventSystem;
    delete sGraphicsHandler;

    delete mDebugWindow;
}

void Engine::Start(Game *game)
{
    EASY_MAIN_THREAD;
    EASY_PROFILER_ENABLE;

    Logger().log("Tronus Engine Starting...", LogLevel::WARNING);

    if (!(mGame = game)) {
        Logger().log("No game instance provied aborting...", LogLevel::ERROR);
        return;
    }

    Logger().log("Starting Subsystem Initialization...");
    sLog->Initialize(this);
    sJobHandler->Initialize(this);
    sResourceManager->Initialize(this);
    sProfiler->Initialize(this);
    sEventSystem->Initialize(this);
    sGraphicsHandler->Initialize(this);

    if (mGame->EngineCreateWindow)
        sGraphicsHandler->CreateWindow(CreateWindowInfo());

    Logger().log("Starting Subsystem Post Initialization...");
    sLog->PostInit();
    sJobHandler->PostInit();
    sResourceManager->PostInit();
    sProfiler->PostInit();
    sEventSystem->PostInit();
    sGraphicsHandler->PostInit();

    mWorld = new World(sEventSystem);
    mGame->PreWorldStartUp(*this);

    mRunning = true;

    mDebugWindow = new DebugWindow(*this);
    sEventSystem->AddListener(this);

    sEventSystem->AddListener(sLog);
    sEventSystem->AddListener(sJobHandler);
    sEventSystem->AddListener(sResourceManager);
    sEventSystem->AddListener(sProfiler);
    sEventSystem->AddListener(sEventSystem);
    sEventSystem->AddListener(sGraphicsHandler);

    /* mWorld->StartWorld(); */

    mGame->OnWorldLoad(*mWorld);

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
    sEventSystem->Shutdown();
    sLog->Shutdown();
    Logger().log("All subsystems have been stopped");
}

void Engine::Tick()
{
    EASY_FUNCTION(profiler::colors::Red);

    sLog->Tick();
    sJobHandler->Tick();
    sResourceManager->Tick();
    sProfiler->Tick();
    sEventSystem->Tick();
    sGraphicsHandler->Tick();

    mWorld->Update();
}

std::vector<int> tr::Engine::SubscripeTo() const { return { ENGINE_CHANNEL }; }

void Engine::OnEvent(const Event &e, int channel)
{
    if (e.Identifier == event::WINDOW) {
        const auto &we = static_cast<const WindowEvent &>(e);
        if (we.type == WindowEvent::CLOSED) {
            this->Stop();
        }
    }
}
