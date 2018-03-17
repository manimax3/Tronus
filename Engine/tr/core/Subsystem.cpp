#include "Subsystem.h"
#include "Engine.h"
#include "../event/EventSystem.h"

using namespace tr;

uint BaseSubsystem::TypeCounter = 0;

bool BaseSubsystem::Initialize(Engine *engine)
{
    mEngine      = engine;
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

std::vector<int> tr::BaseSubsystem::SubscripeTo() const
{
    return { ENGINE_CHANNEL };
}
