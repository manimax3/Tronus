#pragma once

#include "tr.h"
#include "easy/profiler.h"
#include "core/Subsystem.h"

namespace tr
{
    class Profiler : public Subsystem<Profiler>
    {
    public:
        bool Initialize(Engine *engine) override;
    
        bool Shutdown() override;
    
    };
}