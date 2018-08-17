#include "SpriteComponent.h"

#include "../core/Engine.h"
#include "../filesystem/ResourceManager.h"
#include "../graphics/GraphicsHandler.h"
#include "../graphics/Texture.h"
#include "World.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/matrix_transform_2d.hpp"

void tr::SpriteComponent::OnWorldEnter(World &world)
{
    auto &renderer = world.GetEngine().sGraphicsHandler->GetRenderer2D();
    mRenderer      = &renderer;
    SetupRenderState(); // We have the rendere now we can setup a state.
}

void tr::SpriteComponent::ChangeVisibleTo(bool visible)
{
    if (visible != mVisible) {
        mVisible = visible;
        UpdateRenderState();
    }
}

void tr::SpriteComponent::SetUVs(Rect rect)
{
    mUVs = rect;
    UpdateRenderState();
}

void tr::SpriteComponent::SetUVs(Vec2 pos, Vec2 size)
{
    SetUVs(Rect(pos, size));
}

void tr::SpriteComponent::SetSize(Vec2 size)
{
    mSize = size;
    UpdateRenderState();
}

void tr::SpriteComponent::SetColor(Vec4 color)
{
    mColor = color;
    UpdateRenderState();
}

void tr::SpriteComponent::SetTexture(std::string_view name)
{
    auto *const rm  = tr::Engine::Get().sResourceManager;
    const auto  res = rm->GetResource(name);

    if (!res)
        return;

    auto res_t = ResCast<Texture>(res);

    if (!res_t)
        return;

    mTexture = std::move(res_t);
    UpdateRenderState();
}

void tr::SpriteComponent::SetTexture(ResourcePtr<Texture> res)
{
    mTexture = std::move(res); // Just set it and update if its null who cares.
    UpdateRenderState();
}

void tr::SpriteComponent::ResetTexture()
{
    if (!mTexture)
        return;

    mTexture = ResourcePtr<Texture>();
    UpdateRenderState();
}

void tr::SpriteComponent::ForceRenderStateUpdate() { UpdateRenderState(); }

void tr::SpriteComponent::SetupRenderState()
{
    if (!mRenderer)
        return; // Without the renderer we cant do much

    if (mRenderState != 0) {
        // Look like we already have some state going on. This really shouldnt
        // ever be the case. But we will handle it anyway.
        mRenderer->DeleteRenderable(mRenderState);
        mRenderState = 0;
    }

    // Just create a default renderable and submit it to the renderer.
    Renderer2D::Renderable renderable;
    mRenderState = mRenderer->SubmitRenderable(renderable);
}

void tr::SpriteComponent::UpdateRenderState()
{
    if (!mRenderer || mRenderState == 0) {
        // Not that much we can do without compromising performance.
        // Someone should have called SetupRenderState()
        return;
    }

    auto *const r_ptr = mRenderer->ModifyRenderable(mRenderState);

    if (!r_ptr)
        return;

    auto &renderable = *r_ptr;

    renderable.visible = mVisible;
    renderable.color   = mColor;
    renderable.texture = mTexture ? mTexture.get() : nullptr;
    renderable.layer   = GetLayer();

    if (renderable.texture) {
        const auto r  = GetUV();
        const auto ts = renderable.texture->GetSize();

        const Vec4 rel_uv{ r.pos.x / ts.x, r.pos.y / ts.y,
                           (r.pos.x + r.size.x) / ts.x,
                           (r.pos.y + r.size.y) / ts.y };

        renderable.uv = rel_uv;
    }

    // Create the transform matrix
    math::mat3 transform(1.f);
    transform = math::translate(transform, GetAbsolutePosition2D());
    transform = math::scale(transform, GetAbsoluteScale2D());
    transform = math::rotate(transform, GetAbsoluteRotation2D().get());

    const auto size = GetSize();

    renderable.top_left     = transform * Vec3{ 0.f, 0.f, 1.f };
    renderable.top_right    = transform * Vec3{ size.x, 0.f, 1.f };
    renderable.bottom_left  = transform * Vec3{ 0.f, size.y, 1.f };
    renderable.bottom_right = transform * Vec3{ size.x, size.y, 1.f };
}

void tr::SpriteComponent::OnRelativeChange() { UpdateRenderState(); }

void tr::FlipbookComponent::OnWorldEnter(World &world)
{
    SpriteComponent::OnWorldEnter(world);
    TickingComponent = true;
}

void tr::FlipbookComponent::OnComponentUpdate()
{
    if (!AutoAnimate)
        return;

    mMsUntilNext -= Engine::UPDATE_MS_DELTA;

    if (mMsUntilNext < 0) {
        NextFrame();
    }
}

void tr::FlipbookComponent::NextFrame()
{
    mCurrentFrame += 1;
    mCurrentFrame = mCurrentFrame % mFrames.size();

    auto it = mFrames.begin();
    std::advance(it, mCurrentFrame);

    SetUVs(std::get<1>(*it));
    mMsUntilNext = std::get<0>(*it);
}

int tr::FlipbookComponent::AddFrame(Rect rect, int duration, int position_hint)
{
    if (position_hint < 0
        || static_cast<uint>(position_hint) >= mFrames.size()) {
        mFrames.emplace_back(std::tuple{ duration, rect });
        return mFrames.size();
    } else {
        auto it = std::begin(mFrames);
        std::advance(it, position_hint);
        mFrames.emplace(it, std::tuple{ duration, rect });
        return position_hint;
    }
}

void tr::FlipbookComponent::RemoveFrame(int position)
{
    auto it = std::begin(mFrames);
    std::advance(it, position);
    mFrames.erase(it);
}
