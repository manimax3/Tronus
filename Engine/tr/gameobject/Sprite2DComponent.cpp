#include "Sprite2DComponent.h"
#include "../core/Engine.h"
#include "../graphics/GraphicsHandler.h"
#include "../graphics/Renderer2D.h"

namespace {
void ApplyToRenderable(const tr::SpriteRenderSystem::ComponentTuple &tuple,
                       tr::Renderer2D::Renderable &                  r)
{

    const auto [scene_comp, sprite_comp] = tuple;

    r.visible = sprite_comp->visible;
    r.uv      = sprite_comp->uv;
    r.color   = sprite_comp->color;

    tr::Vec3 tl = scene_comp->position;
    tr::Vec3 tr = scene_comp->position + tr::Vec3(sprite_comp->size.x, 0, 0);
    tr::Vec3 bl = scene_comp->position + tr::Vec3(0, sprite_comp->size.y, 0);
    tr::Vec3 br = scene_comp->position
        + tr::Vec3(sprite_comp->size.x, sprite_comp->size.y, 0);

    tl = scene_comp->transform * tl;
    tr = scene_comp->transform * tr;
    br = scene_comp->transform * br;
    bl = scene_comp->transform * bl;

    r.top_left     = { tl.x, tl.y };
    r.top_right    = { tr.x, tr.y };
    r.bottom_left  = { bl.x, bl.y };
    r.bottom_right = { br.x, br.y };

    r.texture = sprite_comp->texture;
}
}

void tr::SpriteRenderSystem::OnGameObjectAdd(GameObjectHandle handle)
{
    auto &                 comps = mRegisteredGameObjects[handle];
    Renderer2D::Renderable r;

    ApplyToRenderable(comps, r);

    if (!mWorld.mEngine)
        return;

    const auto id
        = mWorld.mEngine->sGraphicsHandler->GetRenderer2D().SubmitRenderable(r);

    std::get<Sprite2DComponent *>(comps)->renderable_id = id;
}

void tr::SpriteRenderSystem::OnGameObjectRemove(GameObjectHandle handle)
{
    auto comp = std::get<Sprite2DComponent *>(mRegisteredGameObjects[handle]);

    if (!mWorld.mEngine)
        return;

    mWorld.mEngine->sGraphicsHandler->GetRenderer2D().DeleteRenderable(
        comp->renderable_id);

    comp->renderable_id = 0;
}

void tr::SpriteRenderSystem::OnUpdate()
{
    ForEachTuple([&](ComponentTuple &tuple) {
        auto &sprite_comp = std::get<Sprite2DComponent *>(tuple);

        if (sprite_comp->dirty) {
            auto &r = *mWorld.mEngine->sGraphicsHandler->GetRenderer2D()
                           .ModifyRenderable(sprite_comp->renderable_id);
            ApplyToRenderable(tuple, r);
            sprite_comp->dirty = false;
        }

        return true;
    });
}
