#pragma once
#include "../event/EventListener.h"
#include "GameObject.h"
#include <tr.h>
#include <vector>

namespace tr {
class World{
public:
    World(class Engine *engine);

    void StartWorld();
    void StopWorld();

    template<
        typename G,
        typename = std::enable_if_t<
            std::is_base_of_v<
                GameObject,
                G> && std::is_constructible_v<G, World *, const std::string &, Mat4>>>
    std::weak_ptr<G> SpawnGameObject(const std::string &name,
                                     Mat4 transform = Mat4::Identity())
    {
        mGameObjects.emplace_back(new G(this, name, transform));
        mGameObjects.back()->EnterWorld();
        return std::weak_ptr<G>(
            std::static_pointer_cast<G>(mGameObjects.back()));
    }

    void DispatchTick();
    void RenderDebug();

public:
    class Engine *mEngine = nullptr;

private:
    using GOPtr = std::shared_ptr<GameObject>;
    std::vector<GOPtr> mGameObjects;
};
};
