#pragma once
#include "../math/Rect.h"
#include "Component2D.h"

namespace tr {

/**
 * Component which can collide with other colliders
 */
class ColliderComponent2D : public Component2D {
public:
    /**
     * Enables the physics simulation of this component.
     */
    bool EnablePhysicsSimulation = false;

    /**
     * Add a rectangle shape to this collider.
     * @param rect The rectangle. (pos of the rect is local)
     * @param friction The shapes friction
     * @param density The shapes density
     * @param restitution also know as elasticity
     */
    void AddShapeRect(Rect  rect,
                      float friction    = 0.2f,
                      float density     = 0.1f,
                      float restitution = 0.f);

    /**
     * Add a circle shape to this collider
     */
    void AddShapeCircle(float radius,
                        Vec2  position    = { 0.f, 0.f },
                        float friction    = 0.2f,
                        float density     = 0.1f,
                        float restitution = 0.f);

    /**
     * Setup the component
     */
    void OnWorldEnter(World &world) override;

    /**
     * registers itself on the physics simulation.
     * has to be done post enter because the user has to specify the shape of
     * the collider first.
     */
    void PostWorldEnter() override;

    /**
     * If this components position gets changed we need to update
     * the position internally
     */
    void OnRelativeChange() override;

    /**
     * We need to update from the simulation into the engine
     */
    void OnComponentUpdate() override;

protected:
    /**
     * Internal reprensentation
     */
    void *mBody = nullptr;

    bool DisableCopyAgainSemantic = false;
};
}
