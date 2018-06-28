#pragma once

#include "../event/EventListener.h"
#include "tr.h"
#include <string>

namespace tr {
class Engine;

class BaseSubsystem : public EventListener {
public:
    virtual bool               Initialize(Engine *engine);
    virtual void               PostInit(){};
    virtual bool               Tick();
    virtual bool               Shutdown();
    virtual inline std::string GetName() const { return "UNKNOWN"; }

    inline bool    IsInitialized() const { return mInitialized; }
    inline Engine &GetEngine() { return *mEngine; }

    std::vector<int> SubscripeTo() const override;
    void             OnEvent(const Event &e, int channel) override {}

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

