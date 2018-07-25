#pragma once
#include "../core/Engine.h"
#include <tr.h>

namespace tr {
class Game {
public:
    Game() = default;
    virtual ~Game() = default;

    virtual void PreWorldStart(World &world) {}
    virtual void PreWorldLoad(World &world) {}
    virtual void OnWorldShutdown() {}

    // Configure the Engine
    bool        EngineCreateWindow   = true;
    bool        EngineWindowFromFile = false;
    std::string EngineAssetPrefix    = "assets/";
};
}
