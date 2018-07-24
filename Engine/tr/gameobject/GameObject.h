#pragma once
#include <tr.h>

#include "Component.h"

#include <unordered_map>

namespace tr {

/**
 * Just a convenience typedef
 */
using ComponentUPtr = std::unique_ptr<GameObjectComponent>;

/**
 * Represents a Entity in a world.
 * Functionaliy is added by subclassing and adding components.
 */
class GameObject {
public:
    /**
     * Default Constructor
     */
    explicit GameObject() = default;

    /**
     * Default Destructor
     */
    virtual ~GameObject() = default;

    GameObject(GameObject &&) = default;
    GameObject &operator=(GameObject &&) = default;

    /**
     * Access to the RootComponent
     *
     * If no root component has been set upon calling this function a default
     * SceneComponent will be returned.
     *
     * @return Ref to the RootComponent
     */
    GameObjectComponent &GetRootComponent();

    /**
     * Tells the GameObject which world it belongs to.
     *
     * You dont need the call this. If you want to do something on entering the
     * world override the OnWordEnter member function.
     *
     * @param[in] world The owning world.
     */
    void EnterWorld(class World &world);

    /**
     * Update the component. Gets called by the owning world.
     */
    void Update();

    /**
     * Returns if the gamobject requires ticking updates.
     * If false no component will be ticked either.
     */
    bool IsGameObjectTicking() const { return TickingGameObject; };

    /**
     * Returns if this gameobject has a root component
     */
    bool HasRootComponent() const { return RootComponent; };

    /**
     * Returns a refernence to the owning world.
     * Throws a runtime exception if no world owns this gameobject.
     */
    World &GetWorld();

protected:

    /**
     * Override this if you want to do something upon world entering
     */
    virtual void OnWorldEnter(){};

    /**
     * Override to do something every "update". Only gets called if
     * TickingGameObjects is set to false.
     */
    virtual void OnUpdate(){};

    /**
     * Creates a new component.
     *
     * @param[in] name The name of the component.
     * @return non-owning ptr to the new component.
     */
    template<typename T>
    T *CreateComponent(const std::string &name)
    {
        static_assert(std::is_base_of_v<GameObjectComponent, T>);

        mComponents[name] = std::make_unique<T>();
        auto* ptr = mComponents[name].get();
        ptr->SetOwner(*this);
        ptr->OnWorldEnter(GetWorld());

        return static_cast<T*>(ptr);
    }

    /**
     * The GameObjects's RootComponent.
     *
     * Represents the root of the component hierachy.
     */
    GameObjectComponent *RootComponent = nullptr;

    /**
     * If the game object should be ticked.
     *
     * If set to true the OnUpdate() method gets called every update
     */
    bool TickingGameObject = false;

private:
    using ComponentMap  = std::unordered_map<std::string, ComponentUPtr>;
    ComponentMap mComponents;
    World *      mWorld = nullptr;
};
}
