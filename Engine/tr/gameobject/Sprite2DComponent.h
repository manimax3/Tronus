#pragma once
#include <tr.h>

#include "../math/Math.h"
#include "Component.h"

namespace tr {
class Sprite2DComponent : public SceneComponent {

public:
    void OnWorldEnter(World &world) override;

protected:
    void OnComponentUpdate() override;

    bool visible       = true;
    uint renderable_id = 0;
    Vec4 color         = Vec4(1.f);
    Vec4 uv            = { 0.f, 0.f, 1.f, 1.f };
    Vec2 size          = { 50.f, 50.f };
    bool dirty         = false;

    class Texture *texture = nullptr;
};

class SpriteRenderSystem : public System<SceneComponent, Sprite2DComponent> {
public:
    explicit SpriteRenderSystem(World &w)
        : System<SceneComponent, Sprite2DComponent>(w)
    {
    }

    void OnGameObjectAdd(GameObjectHandle handle) override;
    void OnGameObjectRemove(GameObjectHandle handle) override;
    void OnUpdate() override;
};
}
