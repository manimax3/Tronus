#pragma once

#include "tr.h"
#include <type_traits>
#include "JobHandler.h"
#include "debug/Profiler.h"

namespace tr
{
    class Engine
    {
    private:
        std::unique_ptr<JobHandler> mJobHandler;
        std::unique_ptr<Profiler> mProfiler;
        
        bool mRunning = false;
        int mUPS = 0;
        
    public:
        Engine();
        void Start();
        void Stop();
        
        template<typename T>
        T* GetSystem()
        {
            static_assert(std::is_base_of_v<Subsystem, T>, "Tried to get an unknown Subsystem");
            
            if constexpr(std::is_same_v<T, JobHandler>)
                    return mJobHandler.get();
            else if constexpr(std::is_same_v<T, Profiler>)
                    return mProfiler.get();
        };
        
    private:
        void Tick();
    };
}