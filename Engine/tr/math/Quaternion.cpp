#include "Quaternion.h"

using namespace tr;

Quaternion::Quaternion(float w /*= 1*/, float x /*= 0*/, float y /*= 0*/, float z /*= 0*/)
    : w(w)
    , x(x)
    , y(y)
    , z(z)
{
}

void Quaternion::rotation(float radians, const Vec3 &axis)
{
    float a   = radians / 2.f;
    this->w   = std::cos(a);
    this->vec = axis.normalize() * std::sin(a);
    this->normalize();
}

void Quaternion::set(float scalar)
{
    this->w = scalar;
}

void Quaternion::set(const Vec3 &vector)
{
    this->vec = vector;
}

Quaternion Quaternion::conjuagte() const
{
    return Quaternion(this->w, -this->x, -this->y, -this->z);
}

Quaternion Quaternion::normalized() const
{
    float l = length();
    return Quaternion(this->w / l, this->vec / l);
}

float Quaternion::length() const
{
    return std::sqrt(w * w + x * x + y * y + z * z);
}

void Quaternion::normalize()
{
    this->w /= length();
    this->vec /= length();
}

Quaternion Quaternion::multiply(const Quaternion &other) const
{
    return Quaternion(this->w * other.w - this->x * other.x - this->y * other.y - this->z * other.z,
        this->w * other.x + this->x * other.w + this->y * other.z - this->z * other.y,
        this->w * other.y - this->x * other.z + this->y * other.w + this->z * other.x,
        this->w * other.z + this->x * other.y - this->y * other.x + this->z * other.w);
}

float Quaternion::dot(const Quaternion &other) const
{
    return std::sqrt(w * other.w + x * other.x + y * other.y + z * other.z);
}

Quaternion Quaternion::add(const Quaternion &other) const
{
    return Quaternion(this->w + other.w, this->vec + other.vec);
}

Quaternion Quaternion::subtract(const Quaternion &other) const
{
    return Quaternion(w - other.w, vec - other.vec);
}

Quaternion Quaternion::divide(float scalar) const
{
    return Quaternion(w / scalar, vec / scalar);
}

Quaternion Quaternion::multiply(float scalar) const
{
    return Quaternion(this->w * scalar, this->vec * scalar);
}

Quaternion Quaternion::Rotation(float radians, const Vec3 &axis)
{
    float a   = radians / 2.f;
    float w   = std::cos(a);
    Vec3  vec = axis.normalize() * std::sin(a);
    return Quaternion(w, vec).normalized();
}

Quaternion Quaternion::FromEuler(float pitch, float yaw, float roll)
{
    auto _yaw   = Quaternion::Rotation(yaw, Vec3(0, 1, 0));
    auto _pitch = Quaternion::Rotation(pitch, Vec3(1, 0, 0));
    auto _roll  = Quaternion::Rotation(roll, Vec3(0, 0, 1));

    return _yaw * _pitch * _roll;
}

Quaternion Quaternion::Interpolate(const Quaternion &start, const Quaternion &end, float t)
{
    auto q1 = start.normalized();
    auto q2 = end.normalized();

    float cos_half_theta = q1.dot(q2);
    if (std::abs(cos_half_theta >= 1.0f))
        return q1;

    float half_theta     = std::acos(cos_half_theta);
    float sin_half_theta = std::sin(half_theta);

    if (std::fabs(sin_half_theta) < 0.001)
        return (q1 + q2).divide(2).normalized();

    float ratioA = std::sin((1.f - t) * half_theta) / sin_half_theta;
    float ratioB = std::sin(t * half_theta) / sin_half_theta;

    return (q1.multiply(ratioA) + q2.multiply(ratioB)).normalized();
}

Vec3 Quaternion::multiply(const Vec3 &other) const
{
    auto c = this->conjuagte();

    Quaternion q1(0, other);
    q1 = this->multiply(q1);
    q1 = q1.multiply(c);
    //qxq*

    return Vec3(q1.x, q1.y, q1.z);
}

Quaternion::Quaternion(float scalar, const Vec3 &vector)
    : w(scalar)
    , vec(vector)
{
}

Quaternion Quaternion::operator*(const Quaternion &rhs)
{
    return this->multiply(rhs);
}

Vec3 Quaternion::operator*(const Vec3 &rhs)
{
    return this->multiply(rhs);
}

Quaternion Quaternion::operator+(const Quaternion &rhs)
{
    return Quaternion(this->w + rhs.w, this->vec + rhs.vec);
}

Quaternion Quaternion::operator-(const Quaternion &rhs)
{
    return Quaternion(this->w - rhs.w, this->vec - rhs.vec);
}
