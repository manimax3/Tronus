#include "Engine.h"
#include "../event/CommonEvents.h"
#include "../util/Keys.h"
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
    delete sLog;
    delete sJobHandler;
    delete sResourceManager;
    delete sProfiler;
    delete sEventSystem;
    delete sGraphicsHandler;
}

void Engine::Start()
{
    EASY_MAIN_THREAD;
    EASY_PROFILER_ENABLE;

    Logger().log("Tronus Engine Starting...", LogLevel::WARNING);

    Logger().log("Starting Subsystem Initialization...");
    sLog->Initialize(this);
    sJobHandler->Initialize(this);
    sResourceManager->Initialize(this);
    sProfiler->Initialize(this);
    sEventSystem->Initialize(this);
    sGraphicsHandler->Initialize(this);


    Logger().log("Starting Subsystem Post Initialization...");
    sLog->PostInit();
    sJobHandler->PostInit();
    sResourceManager->PostInit();
    sProfiler->PostInit();
    sEventSystem->PostInit();
    sGraphicsHandler->PostInit();

    mRunning = true;

    sEventSystem->AddListener(this);

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
    mRunning = false;
    sProfiler->Shutdown();
    sEventSystem->Shutdown();
    sGraphicsHandler->Shutdown();
    sResourceManager->Shutdown();
    sJobHandler->Shutdown();
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
}

std::vector<int> tr::Engine::SubscripeTo() const { return { ENGINE_CHANNEL }; }

void Engine::OnEvent(const Event &e, int channel)
{
    if (e.Identifier == event::INPUT) {
        const auto &ie = static_cast<const InputEvent &>(e);
        if (ie.type == InputEvent::Keyboard && ie.action == InputEvent::PRESS
            && ie.Key == KEY_F3)
            Logger().log("UPS: "s + std::to_string(mLastUps)
                         + " FPS: " + std::to_string(mLastFps));
        else if (ie.type == InputEvent::Keyboard
                 && ie.action == InputEvent::PRESS && ie.Key == KEY_F4) {
            auto *rm = sResourceManager;
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
