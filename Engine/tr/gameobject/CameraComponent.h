#pragma once
#include <tr.h>

#include "../event/Signal.h"
#include "../math/Rect.h"
#include "../util/enum.h"
#include "Component.h"

namespace tr {

BETTER_ENUM(CameraTarget, int, Screen3D, Screen2D);
BETTER_ENUM(CameraMode, int, Perspective, Orthographic);

class CameraComponent : public SceneComponent {
public:
    ~CameraComponent();
    void PostWorldEnter() override;

    Mat4 GetView() const;
    Mat4 GetProjection() const;

    /**
     * If true automatically activates the camera on the specified target.
     * If multiple cameras do this the final active camera is undefined.
     */
    bool AutoActivate = true;

    /**
     * Auto adjust the aspect ration to the target.
     * Used for persepctive mode.
     */
    bool AutoAdjustAspect = true;

    /**
     * Auto adjust the screen rect.
     * Used for orthographic mode.
     */
    bool AutoAdjustScreenRect = true;

    /**
     * The fov value in radians.
     * Used for perspective mode.
     */
    Radians Fov = Radians(math::radians(90.f));

    /**
     * Default aspect ratio.
     */
    float AspectRatio = 16.f / 9.f;

    /**
     * Near plane
     */
    float Near = 0.1f;

    /**
     * Far Plane
     */
    float Far = 100.f;

    /**
     * The screen rect.
     * Used for orthographic mode.
     */
    Rect ScreenRect = Rect(Vec2(0.f, 0.f), Vec2(1920.f, 720.f));

    /**
     * The target of the camera.
     */
    CameraTarget Target = CameraTarget::Screen3D;

    /**
     * The cameras mode.
     * Used to decide whether to produce a perspective or orhographic
     * Projection.
     */
    CameraMode Mode = CameraMode::Perspective;

protected:
    void OnRelativeChange() override;

private:
    void CalculateView();
    void CalculateProjection();

    Mat4                   mView;
    Mat4                   mProjection;
    sig::scoped_connection mConn;
};
}
