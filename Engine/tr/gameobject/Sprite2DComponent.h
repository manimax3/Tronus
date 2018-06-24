#pragma once
#include <tr.h>

#include "DefaultComponents.h"
#include "System.h"

namespace tr {
struct Sprite2DComponent {
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
