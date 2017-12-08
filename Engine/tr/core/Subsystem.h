#pragma once

#include "tr.h"

namespace tr {
class Engine;

class BaseSubsystem {
public:
    virtual bool Initialize(Engine *engine);
    virtual bool Tick();
    virtual bool Shutdown();

    inline bool IsInitialized() const { return mInitialized; }

protected:
    bool    mInitialized = false;
    Engine *mEngine;

    static uint TypeCounter;
};

template<typename T>
class Subsystem : public BaseSubsystem {
public:
    static uint GetTypeID()
    {
        static uint _typeID = BaseSubsystem::TypeCounter++;
        return _typeID;
    }
};
}

