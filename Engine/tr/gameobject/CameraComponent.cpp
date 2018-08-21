#include "CameraComponent.h"
#include "../core/Engine.h"
#include "../graphics/ForwardRenderer.h"
#include "../graphics/GraphicsHandler.h"
#include "GameObject.h"
#include "InputComponent.h"

void tr::CameraComponent::PostWorldEnter()
{
    tr::Engine::Get().sGraphicsHandler->CameraUpdate(this, false);
    mConn = tr::Engine::Get().sGraphicsHandler->WindowChanged.connect(
        [&](const WindowEvent &e) {
            if (e.type != WindowEvent::RESIZED)
                return;

            if (AutoAdjustAspect) {
                AspectRatio = (float)e.xSize / e.ySize;
            }
            if (AutoAdjustScreenRect) {
                ScreenRect = Rect(0, 0, e.xSize, e.ySize);
            }

            CalculateProjection();
        });
    CalculateView();
    CalculateProjection();
}

tr::CameraComponent::~CameraComponent()
{
    tr::Engine::Get().sGraphicsHandler->CameraUpdate(this, true);
}

tr::Mat4 tr::CameraComponent::GetView() const { return mView; }

tr::Mat4 tr::CameraComponent::GetProjection() const { return mProjection; }

void tr::CameraComponent::CalculateView()
{
    Vec4       forward{ 0.f, 0.f, -1.f, 1.f };
    const Vec4 up{ 0.f, 1.f, 0.f, 1.f };
    const auto pos = GetAbsoluteTranslation();
    const auto abs = GetAbsoluteTransform();

    forward = math::normalize(abs * forward);

    mView = glm::lookAt(pos, pos + Vec3(forward), Vec3(up));
}

void tr::CameraComponent::CalculateProjection()
{
    if (Mode == +CameraMode::Perspective) {
        mProjection = math::perspective(Fov.get(), AspectRatio, Near, Far);
    } else if (Mode == +CameraMode::Orthographic) {
        mProjection = math::ortho(ScreenRect.pos.x, ScreenRect.pos.y,
                                  ScreenRect.size.x, ScreenRect.size.y);
    }
}

void tr::CameraComponent::OnRelativeChange() { CalculateView(); }
