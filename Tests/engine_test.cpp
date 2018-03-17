#include "catch.hpp"
#include "tr/core/Engine.h"
#include "tr/core/JobHandler.h"


TEST_CASE("Engine Creation and Running")
{
    tr::Engine engine;
    
    tr::JobHandler *handler = engine.sJobHandler;
    REQUIRE(handler);
    
    //engine.Start();
    
    // Cant stop the engine from here right now
    //engine.Stop();
}
