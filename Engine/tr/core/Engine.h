#pragma once

#include "../util/Log.h"
#include "tr.h"
#include <map>
#include <type_traits>

namespace tr {
class Engine : public EventListener {
public:
    class Log *            sLog             = nullptr;
    class JobHandler *     sJobHandler      = nullptr;
    class ResourceManager *sResourceManager = nullptr;
    class Profiler *       sProfiler        = nullptr;
    class EventSystem *    sEventSystem     = nullptr;
    class GraphicsHandler *sGraphicsHandler = nullptr;

    class World *mWorld = nullptr;

private:
    bool mRunning = false;
    int  mUPS     = 0;
    int  mLastUps = 0;

    int mFPS     = 0;
    int mLastFps = 0;

    class DebugWindow *mDebugWindow = nullptr;
    class Game &       mGame;

public:
    Engine(class Game &game);
    ~Engine() override;
    void Start();
    void Stop();

    inline Log &       Logger() { return *sLog; }
    inline const Game &GetGame() const { return mGame; }

    // EventListener
    std::vector<int> SubscripeTo() const override;
    void             OnEvent(const Event &e, int channel) override;

    friend class DebugWindow;

private:
    void Tick();
};
}
