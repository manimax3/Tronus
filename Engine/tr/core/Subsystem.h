#pragma once

#include "tr.h"

namespace tr
{
    class Engine;
    
    class Subsystem
    {
    public:
        virtual bool Initialize(Engine *engine);
        virtual bool Tick();
        virtual bool Shutdown();
        
        inline  bool IsInitialized() const { return mInitialized; }
        
    protected:
        bool mInitialized = false;
        Engine *mEngine;
    };
}

