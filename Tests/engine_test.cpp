#include "catch.hpp"
#include "tr/core/Engine.h"
#include "tr/core/JobHandler.h"
#include "tr/gameobject/Game.h"


TEST_CASE("Engine Creation and Running")
{
    tr::Game game;
    
    tr::JobHandler *handler = tr::Engine::Get().sJobHandler;
    REQUIRE(handler);
    
    //engine.Start();
    
    // Cant stop the engine from here right now
    //engine.Stop();
}
