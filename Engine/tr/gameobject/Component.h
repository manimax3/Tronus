#pragma once
#include <tr.h>

#include "../math/Math.h"
#include "../math/Transform.h"

#include <list>
#include <memory>

namespace tr {

class GameObject;

/**
 * Top-Level Component Class.
 *
 * Only gives the component the capability to have childs and be updated.
 */
class GameObjectComponent {
public:
    explicit GameObjectComponent();
    virtual ~GameObjectComponent();

    GameObjectComponent(GameObjectComponent &&) = default;
    GameObjectComponent &operator=(GameObjectComponent &&) = default;

    /**
     * Attach a component as a child of this component.
     *
     * Registers the passed component as a child of this component. No ownership
     * transfer occurs.
     * @param ptr The component to be child of this
     */
    void AttachChildComponent(GameObjectComponent *ptr);

    /**
     * Unregister a component from being a child of this.
     *
     * @param ptr Which component to be removed. If ptr is not a child of this
     * nothing happens.
     */
    void DetachChildComponent(GameObjectComponent *ptr);

    /**
     * Checks if ptr is a child of this component.
     */
    bool IsChild(GameObjectComponent *ptr) const;

    /**
     * Updates the component.
     *
     * Only happens if TickingComponent is set to true.
     * Finally calls OnComponentUpdate()
     */
    void UpdateComponent();

    /**
     * Checks if this component gets ticked
     */
    bool IsComponentTicking() const { return TickingComponent; };

    /**
     * Checks if this component has a parent component.
     *
     * True does NOT mean that this component is some GameObjects's root
     * component.
     */
    bool HasParent() const { return mParentComponent; };

    /**
     * Set the owning GameObject.
     * Can only be set once.
     */
    void SetOwner(GameObject &owner);

    /**
     * Returns the owning GameObject.
     * If no owner was specified a std::runtime_error it thrown.
     */
    GameObject &GetOwner();

    /**
     * Gets called when the component enters the world.
     * Override this if you want to setup some stuff.
     */
    virtual void OnWorldEnter(class World &world){};

    /**
     * Checks for Positional Capabilities.
     *
     * GameObjectComponents dont represent something "physical" in world so they
     * dont have a transform. This functionality gets added by SceneComponents.
     *
     * DONT override this if you are not subclassing SceneComponent, the engine
     * expects this to be an instance of SceneComponent if this function returns
     * true.
     */
    virtual bool IsComponentRelative() const { return false; };

protected:
    /**
     * Can be overwritten to add functionality during updating.
     */
    virtual void OnComponentUpdate(){};

    /**
     * Enables calling the UpdateComponent() function.
     *
     * Can be change at any given time. But no gurantees when the first update
     * will occur.
     */
    bool TickingComponent = false;

    /**
     * Returns the Parent component.
     */
    GameObjectComponent *GetParent() const { return mParentComponent; };

private:
    std::list<GameObjectComponent *> mChildComponents;
    GameObjectComponent *            mParentComponent = nullptr;
    GameObject *                     mOwner           = nullptr;
};

/**
 * Adds positional functionality to components
 */
class SceneComponent : public GameObjectComponent {
public:
    /**
     * Checks if this components represents a positon relative to his parent or
     * the world root.
     *
     * This function cannot be overwritten. This means that a positional
     * functionality can only be added by subclassing SceneComponent.
     */
    bool IsComponentRelative() const final override
    {
        return RelativeComponent;
    }

    /**
     * Sets the components relative translation
     */
    void SetRelativeTranslation(Vec3 translation);

    /**
     * Sets the components relative rotation
     * @param rotation EulerAngles(pitch, yaw, roll)
     */
    void SetRelativeRotation(EulerAngles rotation);

    /**
     * Sets the relative scale of the component.
     * Note that no scale is (1, 1, 1)
     */
    void SetRelativeScale(Vec3 scale);

    /**
     * Add a change to the components translation.
     */
    void AddRelativeTranslation(Vec3 translation);

    /**
     * Add to the rotation.
     * This changes the roation relative to the previous one. Be aware of that.
     */
    void AddRelativeRotation(EulerAngles rotation);

    /**
     * Add change of scale to the component.
     * Note that this is multiplicative, meaning that adding (2, 2, 2) doubles
     * the overall scale while (1, 1, 1) would do nothing.
     */
    void AddRelativeScale(Vec3 scale);

    /**
     * Returns the current relative translation.
     */
    Vec3 GetRelativeTranslation() const;

    /**
     * Return the current relative rotation.
     */
    EulerAngles GetRelativeRotation() const;

    /**
     * Returns the current relative scale.
     */
    Vec3 GetRelativeScale() const;

    /**
     * Returns the translation relative to the world.
     * Based on parent components.
     */
    Vec3 GetAbsoluteTranslation() const;

    /**
     * Returns the rotation relative to the world.
     * Based on parent components.
     */
    EulerAngles GetAbsoluteRotation() const;

    /**
     * Returns the scale relative to the world.
     * Based on parent components.
     */
    Vec3 GetAbsoluteScale() const;

    /**
     * Returns the components relative transformation as a Matrix
     */
    Mat4 GetRelativeTransform() const;

    /**
     * Returns the components absolute transformation as a Matrix
     */
    Mat4 GetAbsoluteTransform() const;

public:
    /**
     * If this component is relative to something.
     *
     * Set this to false to make this component no express any postion relativ
     * to his parent.
     */
    bool RelativeComponent = true;

protected:

    /**
     * Returns the ParentComponent castet to SceneComponent.
     * If ParentComponent is not relative returns nullptr.
     */
    SceneComponent* GetParentAsSceneComponent() const;

protected:
    Transform mTransform;
};
}
