#pragma once
#include "Vec2.h"
#include "Vec4.h"

namespace tr {
struct Rect {
    Rect(float x0 = 0, float y0 = 0, float x1 = 0, float y1 = 0)
        : pos(x0, y0)
        , size(x1 - x0, y1 - y0)
    {
    }

    Rect(Vec2 pos, Vec2 size)
        : pos(pos)
        , size(size)
    {
    }

    Rect(const Vec4 &v)
        : pos(v.x, v.y)
        , size(v.z, v.w)
    {
    }

    Rect  operator*(float scale) { return { pos, size * scale }; }
    Rect &operator*=(float scale)
    {
        size *= scale;
        return *this;
    }

    /*
     * Maybe some AABB sutff later here
     * */

    inline float GetArea() const { return size.x * size.y; }

    Vec2 pos;
    Vec2 size;
};
}
