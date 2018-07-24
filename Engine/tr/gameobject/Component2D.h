#pragma once
#include <tr.h>

#include "Component.h"

namespace tr {

/**
 * Component repesenting something in the 2D Plane.
 */
class Component2D : public SceneComponent {
public:

    /**
     * Sets the relative postion of the component in 2D space.
     * Similar to the 3D equivalent.
     */
    void SetRelativePostion2D(Vec2 position);

    /**
     * Sets the relative scale of the component in 2D space.
     * Similar to the 3D equivalent.
     */
    void SetRelativeScale2D(Vec2 scale);

    /**
     * Sets the relative rotation of the component in 2D space.
     * Similar to the 3D equivalent.
     */
    void SetRelativeRotation2D(Radians angle);

    /**
     * Returns the 2D relative position of the component.
     */
    Vec2 GetRelativePostion2D() const;

    /**
     * Returns the 2D relative scale of the component.
     */
    Vec2 GetRelativeScale2D() const;

    /**
     * Returns the 2D relative rotation of the component.
     */
    Radians GetRelativeRotation2D() const;

    /**
     * Returns the 2D absolute position.
     * This also take the parents (3D) positon into account.
     */
    Vec2 GetAbsolutePostion2D() const;

    /**
     * Returns the 2D absolute scale.
     * This also take the parents (3D) scale into account.
     */
    Vec2 GetAbsoluteScale2D() const;

    /**
     * Returns the 2D absolute rotation.
     * This also take the parents (3D) rotation into account.
     */
    Radians GetAbsoluteRotation2D() const;

    /**
     * Returns the Layer this component is in
     */
    virtual int GetLayer() const { return mLayer; };

    /**
     * Sets the components Layer.
     */
    virtual void SetLayer(int layer) { mLayer = layer; };

protected:

    /**
     * The components layer
     */
    int mLayer = 0;
};
}
