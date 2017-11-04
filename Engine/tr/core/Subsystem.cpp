#include "Subsystem.h"
#include "Engine.h"

using namespace tr;

uint BaseSubsystem::TypeCounter = 0;

bool BaseSubsystem::Initialize(Engine *engine)
{
    mEngine = engine;
    mInitialized = true;
    return true;
}

bool BaseSubsystem::Tick()
{
    return true;
}

bool BaseSubsystem::Shutdown()
{
    return false;
}