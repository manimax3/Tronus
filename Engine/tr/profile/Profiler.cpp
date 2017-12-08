#include "Profiler.h"

using namespace tr;

bool Profiler::Initialize(Engine *engine)
{
    profiler::startListen();

    return Subsystem::Initialize(engine);
}

bool Profiler::Shutdown()
{
    profiler::stopListen();

    return Subsystem::Shutdown();
}
