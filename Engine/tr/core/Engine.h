#pragma once

#include "../util/Log.h"
#include "../util/Singleton.h"
#include "../event/CommonEvents.h"
#include "tr.h"
#include <map>
#include <type_traits>

namespace tr {
class Engine : public Singleton<Engine> {
public:
    class JobHandler *     sJobHandler      = nullptr;
    class ResourceManager *sResourceManager = nullptr;
    class Profiler *       sProfiler        = nullptr;
    class GraphicsHandler *sGraphicsHandler = nullptr;

    class World *mWorld = nullptr;

    constexpr static double UPDATE_MS_DELTA = 1000.0 / 60.f;
    constexpr static double TICK_MS_DELTA   = UPDATE_MS_DELTA;

private:
    bool mRunning = false;
    int  mUPS     = 0;
    int  mLastUps = 0;

    int mFPS     = 0;
    int mLastFps = 0;

    class DebugWindow *mDebugWindow = nullptr;
    class Game *       mGame        = nullptr;

public:
    Engine();
    ~Engine() override;
    void Start(class Game *game);
    void Stop();

    inline const Game &GetGame() const { return *mGame; }

    void OnEvent(const WindowEvent &e);

    friend class DebugWindow;

private:
    void Tick();
};
}
