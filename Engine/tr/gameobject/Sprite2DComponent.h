#pragma once
#include "../event/EventListener.h"
#include "../math/Math.h"
#include "../math/Rect.h"
#include "Component.h"
#include <tr.h>

namespace tr {
class Sprite2DComponent : public SceneComponent, public EventListener {
public:
    Sprite2DComponent(const std::string &name,
                      GameObject *       owner,
                      SceneComponent *   parent);

    std::vector<int> SubscripeTo() const override;
    void             OnEvent(const Event &e, int channel) override;

    void SetDrawAndTextureBounds(const Rect &r);
    void HandleTextureLoad(const std::string &resource);
    void SetColor(const Vec4 &col);

protected:
    void OnWorldEnter() override;
    void OnWorldLeave() override;

public:
    bool           mVisible = true;
    class Texture *mTexture = nullptr;
    Rect           mDrawBounds;
    Vec4           mUV = Vec4(0.f, 0.f, 1.f, 1.f);
    Vec4           mColor = Vec4(1.f);
};
}
