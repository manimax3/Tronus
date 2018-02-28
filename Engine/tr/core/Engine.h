#pragma once

#include "../util/Log.h"
#include "JobHandler.h"
#include "profile/Profiler.h"
#include "tr.h"
#include <map>
#include <type_traits>

namespace tr {
class Engine {
private:
    std::unordered_map<uint, std::unique_ptr<BaseSubsystem>> mSubsystems;

    bool mRunning = false;
    int  mUPS     = 0;
    Log *mLog     = nullptr;

public:
    Engine();
    void Start();
    void Stop();

    inline Log &Logger() { return *mLog; }

    template<typename T>
    T *GetSystem()
    {
        static_assert(std::is_base_of_v<Subsystem<T>, T>,
            "T is not base of Subsystem<T>");
        assert(HasSystem<T>());

        return static_cast<T *>(mSubsystems.find(T::GetTypeID())->second.get());
    };

    template<typename T, typename... Args>
    T *AddSystem(Args &&... args)
    {
        static_assert(std::is_base_of_v<Subsystem<T>, T>,
            "T is not base of Subsystem<T>");
        assert(!HasSystem<T>());

        mSubsystems.emplace(T::GetTypeID(),
            std::unique_ptr<BaseSubsystem>(new T(std::forward<Args>(args)...)));

        // If the engine is already running init this subsystem directly
        if (mRunning)
            GetSystem<T>()->Initialize(this);

        return GetSystem<T>();
    };

    template<typename T>
    bool HasSystem() const
    {
        static_assert(std::is_base_of_v<Subsystem<T>, T>,
            "T is not base of Subsystem<T>");

        return mSubsystems.find(T::GetTypeID()) != mSubsystems.end();
    }

private:
    void Tick();
};
}
