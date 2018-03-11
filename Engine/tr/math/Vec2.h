#pragma once
#include <tr.h>

namespace tr {
class Vec2 {
public:
    Vec2(float v = 0.f)
        : x(v)
        , y(v)
    {
    }
    Vec2(float x, float y)
        : x(x)
        , y(y)
    {
    }

    Vec2 operator+(const Vec2 &other) const
    {
        return Vec2(x + other.x, y + other.y);
    }
    Vec2 operator+(float v) const { return Vec2(x + v, x + v); }

    Vec2 operator-(const Vec2 &other) const
    {
        return Vec2(x - other.x, y - other.y);
    }
    Vec2 operator-(float v) const { return Vec2(x - v, x - v); }

    Vec2 operator*(float v) const { return Vec2(x * v, x * v); }
    Vec2 operator/(float v) const { return Vec2(x / v, x / v); }

    Vec2 &Normalize()
    {
        const float l = Length();
        x /= l;
        y /= l;
        return *this;
    }

    Vec2 Normalized() const
    {
        Vec2 v = *this;
        v.Normalize();
        return v;
    }

    float Length() const { return std::sqrt(x * x + y * y); }

    Vec2 &operator+=(const Vec2 &other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    Vec2 &operator+=(float v)
    {
        x += v;
        y += v;
        return *this;
    }

    Vec2 &operator-=(const Vec2 &other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    Vec2 &operator-=(float v)
    {
        x -= v;
        y -= v;
        return *this;
    }

    Vec2 &operator*=(float v)
    {
        x *= v;
        y *= v;
        return *this;
    }
    Vec2 &operator/=(float v)
    {
        x /= v;
        y /= v;
        return *this;
    }

    float Dot(const Vec2 &other) const { return x * other.x + y * other.y; }

public:
    float x, y;
};
}
