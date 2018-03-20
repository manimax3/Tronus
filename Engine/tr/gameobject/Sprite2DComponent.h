#pragma once
#include "../event/EventListener.h"
#include "../math/Math.h"
#include "../math/Rect.h"
#include "Component.h"
#include <tr.h>

namespace tr {
class Sprite2DComponent : public SceneComponent{
public:
    Sprite2DComponent(const std::string &name,
                      GameObject *       owner,
                      SceneComponent *   parent);

    void SetDrawAndTextureBounds(const Rect &r);
    void HandleTextureLoad(const std::string &resource);
    void SetColor(const Vec4 &col);

protected:
    void OnWorldEnter() override;
    void OnWorldLeave() override;

    void OnEvent(const Event &e) override;

    void UpdateValues();

public:
    bool           mDirty   = true;
    bool           mVisible = true;
    class Texture *mTexture = nullptr;
    Rect           mDrawBounds;
    Vec2           mOrigin = { 0.f, 0.f };
    Vec4           mUV    = Vec4(0.f, 0.f, 1.f, 1.f);
    Vec4           mColor = Vec4(1.f);

protected:
    Vec2 mTopLeft, mTopRight, mBottomLeft, mBottomRight;
};
}
