#include "Profiler.h"
#include "../core/Engine.h"

using namespace tr;

bool Profiler::Initialize(Engine *engine)
{
    engine->Logger().log("Starting Profiler...");
    profiler::startListen();

    return Subsystem::Initialize(engine);
}

bool Profiler::Shutdown()
{
    profiler::stopListen();

    return Subsystem::Shutdown();
}
