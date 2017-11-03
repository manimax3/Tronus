#pragma once

#include "tr.h"
#include <type_traits>
#include "JobHandler.h"

namespace tr
{
    class Engine
    {
    private:
        std::unique_ptr<JobHandler> mJobHandler;
        
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
        };
        
    private:
        void Tick();
    };
}