#pragma once
#include <tr.h>

#include "Box2D/Box2D.h"
#include "World.h"

namespace tr {

/**
 * World capability for 2d collision handling.
 * Uses Box2D internally
 */
class Collision2DCapability : public WorldCapability {
public:

    /**
     * We use this to advance the physics simulation
     */
    void OnUpdate() override;

    /**
     * We ust this for world creation and other b2 setup
     */
    void OnLoad() override;

    /**
     * Collision2DCapability Name
     */
    std::string GetCapabilityName() const override
    {
        return "Collision2DCapability";
    }

    /**
     * The gravitation the world applies to every object
     */
    Vec2 Gravitation = Vec2{ 0.f, -9.8f };

    /**
     * How many VelocityIterations should be done every update.
     */
    int VelocityIterations = 6;

    /**
     * How many PositionIterations should be done every update.
     */
    int PositionIterations = 2;

private:
    /**
     * The simulated world
     */
    std::unique_ptr<b2World> mBox2DWorld;
};
}
