#pragma once

#include "core/Subsystem.h"
#include "easy/profiler.h"
#include "tr.h"

namespace tr {
class Profiler : public Subsystem<Profiler> {
public:
    bool Initialize(Engine *engine) override;

    bool Shutdown() override;
};
}
