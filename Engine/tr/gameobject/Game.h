#pragma once
#include "../core/Engine.h"
#include <tr.h>

namespace tr {
struct Game {
    virtual ~Game() = default;

    virtual void PreWorldStartUp(Engine &engine) {}
    virtual void OnWorldLoad(World &world) {}
    virtual void OnWorldShutdown() {}
};
}
