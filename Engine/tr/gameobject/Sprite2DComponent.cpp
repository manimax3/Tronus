#include "Sprite2DComponent.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../event/EventSystem.h"
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
    if (e.Identifier != event::RENDER_2D) {
        return;
    }

    if (!mVisible)
        return;

    if (mDirty)
        UpdateValues();

    const Render2DEvent &  re = static_cast<const Render2DEvent &>(e);
    Renderer2D::Renderable r;

    r.top_left     = mTopLeft;
    r.top_right    = mTopRight;
    r.bottom_left  = mBottomLeft;
    r.bottom_right = mBottomRight;

    if (mTexture) {
        r.texture = mTexture.get();
        r.uv      = mUV;
    }

    r.color = mColor;

    re.renderer.Submit(r);
}

void tr::Sprite2DComponent::OnWorldEnter() {}

void tr::Sprite2DComponent::OnWorldLeave() {}

void tr::Sprite2DComponent::UpdateValues()
{
    Vec3 tl = Vec3(mDrawBounds.pos.x, mDrawBounds.pos.y, 0.f);
    Vec3 tr = Vec3(mDrawBounds.pos.x, mDrawBounds.pos.y, 0.f);
    Vec3 bl = Vec3(mDrawBounds.pos.x, mDrawBounds.pos.y, 0.f);
    Vec3 br = Vec3(mDrawBounds.pos.x, mDrawBounds.pos.y, 0.f);

    br += Vec3(mDrawBounds.size.x, mDrawBounds.size.y, 0.f);
    tr += Vec3(mDrawBounds.size.x, 0.f, 0.f);
    bl += Vec3(0.f, mDrawBounds.size.y, 0.f);

    const Vec3 origin3 = Vec3(mOrigin.x, mOrigin.y, 0.f);

    tl -= origin3;
    tr -= origin3;
    br -= origin3;
    bl -= origin3;

    const Mat4 transform = GetFinalTransform();

    tl = transform * Vec4(tl, 1.f);
    tr = transform * Vec4(tr, 1.f);
    bl = transform * Vec4(bl, 1.f);
    br = transform * Vec4(br, 1.f);

    mTopLeft     = Vec2(tl.x, tl.y);
    mTopRight    = Vec2(tr.x, tr.y);
    mBottomLeft  = Vec2(bl.x, bl.y);
    mBottomRight = Vec2(br.x, br.y);

    mDirty = false;
}

void tr::Sprite2DComponent::SetDrawAndTextureBounds(const Rect &r)
{
    mDirty = true;
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
