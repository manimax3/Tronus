#pragma once
#include <tr.h>

namespace tr {
class Vec3;

class TR_API Vec4 {
public:
    // Constructors
    Vec4(const Vec4 &) = default;

    Vec4(float v = 0);
    Vec4(float x, float y, float z, float w = 0);
    Vec4(const Vec3 &, float w = 0);

    // Basic Vector Math
    Vec4 add(float v) const;
    Vec4 add(const Vec4 &other) const;
    Vec4 subtract(float v) const;
    Vec4 subtract(const Vec4 &other) const;
    Vec4 multiply(float v) const;
    Vec4 divide(float v) const;

    float length() const;
    float dot(const Vec4 &other) const;

    Vec4 normalize() const;

    // Operator Overloads
    friend TR_API Vec4 operator+(const Vec4 &left, float v);
    friend TR_API Vec4 operator+(const Vec4 &left, const Vec4 &right);
    friend TR_API Vec4 operator-(const Vec4 &left, float v);
    friend TR_API Vec4 operator-(const Vec4 &left, const Vec4 &right);
    friend TR_API Vec4 operator*(const Vec4 &left, float v);
    friend TR_API Vec4 operator/(const Vec4 &left, float v);

    Vec4 &operator+=(float v);
    Vec4 &operator+=(const Vec4 &other);
    Vec4 &operator-=(float v);
    Vec4 &operator-=(const Vec4 &other);
    Vec4 &operator*=(float v);
    Vec4 &operator/=(float v);

    bool operator==(const Vec4 &) const;
    bool operator!=(const Vec4 &) const;

    Vec4 &operator=(const Vec4 &other) = default;

    float &      operator[](const std::size_t &);
    const float &operator[](const std::size_t &) const;

public:
    union {
        float m_Values[4];
        struct {
            float x, y, z, w;
        };
    };
};
}
