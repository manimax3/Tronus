#include "Component2D.h"

void tr::Component2D::SetRelativePostion2D(Vec2 position)
{
    SetRelativeTranslation({ position.x, position.y, 0.f });
}

void tr::Component2D::SetRelativeRotation2D(Radians angle)
{
    SetRelativeRotation({ 0.f, 0.f, angle.get() });
}

void tr::Component2D::SetRelativeScale2D(Vec2 scale)
{
    SetRelativeScale({ scale.x, scale.y, 0.f });
}

tr::Vec2 tr::Component2D::GetRelativePostion2D() const
{
    const auto pos = GetRelativeTranslation();
    return { pos.x, pos.y };
}

tr::Radians tr::Component2D::GetRelativeRotation2D() const
{
    return Radians(GetRelativeRotation().z);
}

tr::Vec2 tr::Component2D::GetRelativeScale2D() const
{
    const auto scale = GetRelativeScale();
    return { scale.x, scale.y };
}

tr::Vec2 tr::Component2D::GetAbsolutePostion2D() const
{
    const auto abs = GetAbsoluteTranslation();
    return { abs.x, abs.y };
}

tr::Radians tr::Component2D::GetAbsoluteRotation2D() const
{
    const auto abs = GetAbsoluteRotation();
    return Radians(abs.z);
}

tr::Vec2 tr::Component2D::GetAbsoluteScale2D() const
{
    const auto abs = GetAbsoluteScale();
    return { abs.x, abs.y };
}
