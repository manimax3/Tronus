#pragma once
#include "../core/Engine.h"
#include <tr.h>

namespace tr {
struct Game {
    virtual ~Game() = default;

    virtual void PreWorldStartUp(Engine &engine) {}
    virtual void OnWorldLoad(World &world) {}
    virtual void OnWorldShutdown() {}

    // Configure the Engine
    bool        EngineCreateWindow   = true;
    bool        EngineWindowFromFile = false;
    std::string EngineAssetPrefix    = "assets/";
};
}
