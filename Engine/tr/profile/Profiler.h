#pragma once

#include "core/Subsystem.h"
#include "easy/profiler.h"
#include "tr.h"

namespace tr {
class Profiler : public Subsystem<Profiler> {
public:
    bool Initialize(Engine *engine) override;
    inline std::string GetName() const override { return "Profiler"; };

    bool Shutdown() override;
};
}
