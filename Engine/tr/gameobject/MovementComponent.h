#pragma once
#include <tr.h>

#include "../util/Log.h"
#include "Component.h"

namespace tr {
/**
 * Component to express an movement based on a velocity
 */
class MovementComponent : public SceneComponent {
public:
    void OnWorldEnter(World &world) override { TickingComponent = true; }

    /**
     * Sets the linear velocity.
     * In units per second
     */
    void SetVelocity(Vec3 velocity) { Velocity = velocity; }

    /**
     * Adds to the current velocity
     */
    void AddVelocity(Vec3 velocity) { Velocity += velocity; }

    /**
     * Returns the current velocity
     */
    Vec3 GetVelocity() const { return Velocity; }

protected:
    Vec3 Velocity = Vec3(0.f);

    void OnComponentUpdate() override
    {
        if (Velocity.x != 0 || Velocity.y != 0 || Velocity.z != 0)
            AddRelativeTranslation(Velocity * (1.f / 60.f));
    }
};
}
