#pragma once
#include <tr.h>

#include "GameObject.h"
#include <vector>
#include <boost/poly_collection/base_collection.hpp>

namespace tr {

class Engine;
class World;

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

    template<typename T>
    void AttachWorldCapability()
    {
    }

    /**
     * Returns a refernce to the engine.
     */
    Engine &GetEngine() { return mEngine; };

private:
    void SetupInternal(GameObject *ptr);

    Engine &mEngine;

    boost::poly_collection::base_collection<GameObject> mGameObjects;
};
}
