#include "Subsystem.h"
#include "Engine.h"

using namespace tr;

bool Subsystem::Initialize(Engine *engine)
{
    mEngine = engine;
    mInitialized = true;
    return true;
}

bool Subsystem::Tick()
{
    return true;
}

bool Subsystem::Shutdown()
{
    return false;
}