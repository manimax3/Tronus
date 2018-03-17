#include "Sprite2DComponent.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../event/EventSystem.h"
#include "../filesystem/ResourceManager.h"
#include "../graphics/Renderer2D.h"
#include "../graphics/Texture.h"
#include "GameObject.h"
#include "World.h"

tr::Sprite2DComponent::Sprite2DComponent(const std::string &name,
                                         GameObject *       owner,
                                         SceneComponent *   parent)
    : SceneComponent(name, owner, parent)
{
}

void tr::Sprite2DComponent::OnEvent(const Event &e)
{
    if (e.Identifier != event::RENDER_2D)
        return;

    if (!mVisible)
        return;

    const Render2DEvent &  re = static_cast<const Render2DEvent &>(e);
    Renderer2D::Renderable r;
    r.top_left     = mDrawBounds.pos;
    r.bottom_right = mDrawBounds.pos + mDrawBounds.size;
    r.top_right    = mDrawBounds.pos + Vec2(mDrawBounds.size.x, 0.f);
    r.bottom_left  = mDrawBounds.pos + Vec2(0.f, mDrawBounds.size.y);

    if (mTexture) {
        r.texture = mTexture;
        r.uv      = mUV;
    }

    r.color = mColor;

    re.renderer.Submit(r);
}

void tr::Sprite2DComponent::OnWorldEnter() {}

void tr::Sprite2DComponent::OnWorldLeave() {}

void tr::Sprite2DComponent::SetDrawAndTextureBounds(const Rect &r)
{
    mDrawBounds = r;
    mUV = Vec4(r.pos.x, r.pos.y, r.pos.x + r.size.x, r.pos.y + r.size.y);
}

void tr::Sprite2DComponent::SetColor(const Vec4 &col) { mColor = col; }

void tr::Sprite2DComponent::HandleTextureLoad(const std::string &resource)
{
    if (mTexture = mOwner->Context.ResManager->GetRes<Texture>(resource);
        !mTexture) {
        mOwner->Context.ResManager->LoadResource(resource);
        mTexture = mOwner->Context.ResManager->GetRes<Texture>(resource);
    }
}
