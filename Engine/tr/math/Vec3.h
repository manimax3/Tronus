#pragma once
#include <tr.h>

namespace tr {
class TR_API Vec3 {
public:
    // Constructors
    Vec3(const Vec3 &) = default;
    Vec3(Vec3 &&)      = default;

    Vec3(float v = 0);
    Vec3(float x, float y, float z);

    // Basic Vector Math
    Vec3 add(float v) const;
    Vec3 add(const Vec3 &other) const;
    Vec3 subtract(float v) const;
    Vec3 subtract(const Vec3 &other) const;
    Vec3 multiply(float v) const;
    Vec3 divide(float v) const;

    float length() const;
    float dot(const Vec3 &other) const;
    Vec3  cross(const Vec3 &other) const;

    Vec3 normalize() const;

    // Operator Overloads
    Vec3 operator+(float v) const;
    Vec3 operator+(const Vec3 &right) const;
    Vec3 operator-(float v) const;
    Vec3 operator-(const Vec3 &right) const;
    Vec3 operator*(float v) const;
    Vec3 operator/(float v) const;

    Vec3 &operator+=(float v);
    Vec3 &operator+=(const Vec3 &other);
    Vec3 &operator-=(float v);
    Vec3 &operator-=(const Vec3 &other);
    Vec3 &operator*=(float v);
    Vec3 &operator/=(float v);

    bool operator==(const Vec3 &) const;
    bool operator!=(const Vec3 &) const;

    Vec3 &operator=(const Vec3 &) = default;
    Vec3 &operator=(Vec3 &&) = default;

    float &operator[](const std::size_t &);

public:
    union {
        float m_Values[3];
        struct {
            float x, y, z;
        };
    };
};
}

