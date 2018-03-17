#include "catch.hpp"
#include "tr/core/Engine.h"
#include "tr/core/JobHandler.h"
#include "tr/core/Subsystem.h"

int i = 0;

class TestJobHandlder : public tr::Subsystem<TestJobHandlder>
{
public:
    bool Initialize(tr::Engine *engine) override {
        
        return BaseSubsystem::Initialize(engine);
    }
    
    bool Tick() override {
    
        auto* jh = mEngine->sJobHandler;
        jh->AddJob([&](){
            i = 10;
        });
        
        mEngine->Stop();
        
        REQUIRE(i == 10);
        
        return BaseSubsystem::Tick();
    }
};


TEST_CASE("JobHandler", "[jobs]")
{
	//tr::Engine engine;
    //engine.AddSystem<TestJobHandlder>();
    //engine.Start();
}
