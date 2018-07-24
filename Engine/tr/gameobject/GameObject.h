#pragma once
#include <tr.h>

#include "Component.h"
#include "InputComponent.h"

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
     * world override the OnWorldEnter member function.
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
        auto *ptr         = mComponents[name].get();
        ptr->SetOwner(*this);
        ptr->OnWorldEnter(GetWorld());

        return static_cast<T *>(ptr);
    }

    /**
     * The GameObjects's RootComponent.
     *
     * Represents the root of the component hierachy.
     */
    GameObjectComponent *RootComponent = nullptr;

    /**
     * Prevents the cInputComponent from being setup.
     * Does not prevent the construction of cInputComponent.
     *
     * Does not revert the effects of an already setup InputComponent which
     * means that this should optimaly changed during EnterWorld()
     */
    bool DisableInputComponent = false;

    /**
     * Disables the construction of the input component.
     * Changing this only takes affect if done before OnWorldEnter() is called
     * (constructor maybe...) Bu beware the almost no on the input component
     * relying component check for its allocation so only change this if you
     * know what you are doing.
     */
    bool DisableInputComponentConstruction = false;

    /**
     * If the game object should be ticked.
     *
     * If set to true the OnUpdate() method gets called every update
     */
    bool TickingGameObject = false;

private:
    using ComponentMap = std::unordered_map<std::string, ComponentUPtr>;
    ComponentMap mComponents;
    World *      mWorld = nullptr;

    // The input component needs to be desturcted before the components.
    // Which is why there is this extra block
protected:
    /**
     * We use this component to setup our input listenting.
     * For easier lifetime tracking and segfault prevention a shared_ptr is used
     * which can be easily registered with the corresponding signals.
     *
     * The InputComponent can be disabled by setting DisableInputComponent to
     * true. Note that this does not destruct the component itself so you dont
     * have do worry about a Seg Fault.
     */
    std::shared_ptr<InputComponent> cInputComponent;
};
}
