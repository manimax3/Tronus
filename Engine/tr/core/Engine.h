#pragma once

#include "../event/EventSystem.h"
#include "../filesystem/ResourceManager.h"
#include "../graphics/DebugWindow.h"
#include "../graphics/GraphicsHandler.h"
#include "../profile/Profiler.h"
#include "../util/Log.h"
#include "../gameobject/World.h"
#include "JobHandler.h"
#include "tr.h"
#include <map>
#include <type_traits>

namespace tr {
class Engine : public EventListener {
public:
    Log *            sLog             = nullptr;
    JobHandler *     sJobHandler      = nullptr;
    ResourceManager *sResourceManager = nullptr;
    Profiler *       sProfiler        = nullptr;
    EventSystem *    sEventSystem     = nullptr;
    GraphicsHandler *sGraphicsHandler = nullptr;

    World mWorld;

private:
    bool mRunning = false;
    int  mUPS     = 0;
    int  mLastUps = 0;

    int mFPS     = 0;
    int mLastFps = 0;

    DebugWindow *mDebugWindow = nullptr;

public:
    Engine();
    ~Engine();
    void Start();
    void Stop();

    inline Log &Logger() { return *sLog; }

    // EventListener
    std::vector<int> SubscripeTo() const override;
    void             OnEvent(const Event &e, int channel) override;

    friend class DebugWindow;
private:
    void Tick();
};
}
