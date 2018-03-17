#pragma once
#include "../event/EventListener.h"
#include "GameObject.h"
#include <tr.h>
#include <vector>

namespace tr {
class World : public EventListener {
public:
    World(class Engine *engine);

    void StartWorld();
    void StopWorld();

    std::vector<int> SubscripeTo() const override;
    void             OnEvent(const Event &e, int channel) override;

    template<
        typename G,
        typename = std::enable_if_t<
            std::is_base_of_v<
                GameObject,
                G> && std::is_constructible_v<G, World *, const std::string &, Mat4>>>
    G *SpawnGameObject(const std::string &name,
                       Mat4               transform = Mat4::Identity())
    {
        mGameObjects.emplace_back(new G(this, name, transform));
        mGameObjects.back()->EnterWorld();
        return static_cast<G *>(mGameObjects.back().get());
    }

    void DispatchTick();
    void RenderDebug();

public:
    class Engine *mEngine = nullptr;

private:
    using GOPtr = std::unique_ptr<GameObject>;
    std::vector<GOPtr> mGameObjects;
};
};
