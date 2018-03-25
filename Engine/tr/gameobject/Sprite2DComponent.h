#pragma once
#include "../event/EventListener.h"
#include "../filesystem/ResourceManager.h"
#include "../math/Math.h"
#include "../math/Rect.h"
#include "Component.h"
#include <tr.h>

namespace tr {
class Sprite2DComponent : public SceneComponent {
public:
    Sprite2DComponent(const std::string &name,
                      GameObject *       owner,
                      SceneComponent *   parent);

    void SetDrawAndTextureBounds(const Rect &r);
    void HandleTextureLoad(const std::string &resource);
    void SetColor(const Vec4 &col);

    void MarkDirty();

protected:
    void OnWorldEnter() override;
    void OnWorldLeave() override;

    void OnEvent(const Event &e) override;

    void UpdateValues();

public:
    bool                     mVisible = true;
    ResHandle<class Texture> mTexture;
    Rect                     mDrawBounds;
    Vec2                     mOrigin = { 0.f, 0.f };
    Vec4                     mUV     = Vec4(0.f, 0.f, 1.f, 1.f);
    Vec4                     mColor  = Vec4(1.f);
    uint                     mRenderable;

protected:
    bool mDirty = true;
    Vec2 mTopLeft, mTopRight, mBottomLeft, mBottomRight;
};
}
