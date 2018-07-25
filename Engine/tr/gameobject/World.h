#pragma once
#include <tr.h>

#include "GameObject.h"
#include <boost/poly_collection/base_collection.hpp>
#include <vector>

namespace tr {

class Engine;
class World;

/**
 * Used to extend the functionality of the world
 */
class WorldCapability {
public:
    virtual ~WorldCapability() = default;

    /*
     * Call during World Update()
     */
    virtual void OnUpdate(){};

    /*
     * Call during World Start()
     */
    virtual void OnStart(){};

    /*
     * Call during World Load()
     */
    virtual void OnLoad(){};

    /**
     * Returns the name of the capability.
     * Used for debugging and informational purposes.
     */
    virtual std::string GetCapabilityName() const = 0;
};

class World {
public:
    /**
     * World constructor.
     *
     * @param engine Owning Engine.
     */
    explicit World(Engine &engine);

    /**
     * Updates the world.
     *
     * The update gets propagated to the GameObjects.
     */
    void Update();

    /**
     * Starts the world.
     * Called by the engine.
     */
    void Start();

    /**
     * Tells the world to load necesseary resources for starting.
     * Called by the engine. Called before the Games OnWorldLoad function.
     */
    void Load();

    /**
     * Spawns a new GameObject of type T
     */
    template<typename T>
    void Spawn()
    {
        static_assert(std::is_base_of_v<GameObject, T>);
        static_assert(std::is_default_constructible_v<T>);
        auto it = mGameObjects.insert(T());
        SetupInternal(&(*it));
    }

    /**
     * Attach a new capability to this world.
     * Need to be a base of WorldCapability
     */
    template<typename T>
    void AttachWorldCapability()
    {
        static_assert(std::is_base_of_v<WorldCapability, T>);
        static_assert(std::is_default_constructible_v<T>);

        auto *     cap  = new T;
        const auto name = cap->GetCapabilityName();

        if (const auto it = mWorldCapabilities.find(name);
            it == std::end(mWorldCapabilities)) {

            mWorldCapabilities[name] = std::shared_ptr<WorldCapability>(cap);

        } else {
            throw std::runtime_error("Capability of name " + name
                                     + " already exists");
        }
    }

    /**
     * Returns a capability provided its name.
     */
    template<typename T>
    T *GetWorldCapability(const std::string &name)
    {
        static_assert(std::is_base_of_v<WorldCapability, T>);
        static_assert(std::is_default_constructible_v<T>);
        return static_cast<T *>(mWorldCapabilities.at(name).get());
    }

    /**
     * Returns a capability only by type.
     * Does this by creating a temporary capability of type T and then asks it
     * for its name.
     */
    template<typename T>
    T *GetWorldCapability()
    {
        static_assert(std::is_base_of_v<WorldCapability, T>);
        static_assert(std::is_default_constructible_v<T>);

        // We construct a temp capability
        T cap;
        return GetWorldCapability<T>(cap.GetCapabilityName());
    }

    /**
     * Returns a refernce to the engine.
     */
    Engine &GetEngine() { return mEngine; };

private:
    void SetupInternal(GameObject *ptr);

    Engine &mEngine;

    // Just some conevenience type defs
    template<typename T>
    using BaseCollection      = boost::poly_collection::base_collection<T>;
    using WorldCapabilitySPtr = std::shared_ptr<WorldCapability>;
    // ===

    BaseCollection<GameObject>                 mGameObjects;
    std::map<std::string, WorldCapabilitySPtr> mWorldCapabilities;
};
}
