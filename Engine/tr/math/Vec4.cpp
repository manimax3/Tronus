#include "Vec4.h"
#include "Vec3.h"

using namespace tr;

Vec4::Vec4(float v)
    : x(v)
    , y(v)
    , z(v)
    , w(0)
{
}

Vec4::Vec4(const Vec3 &other, float w /*= 0*/)
    : x(other.x)
    , y(other.y)
    , z(other.z)
    , w(w)
{
}

Vec4 Vec4::add(float v) const
{
    return Vec4(this->x + v,
        this->y + v,
        this->z + v,
        this->w + v);
}

Vec4 Vec4::subtract(float v) const
{
    return Vec4(this->x - v,
        this->y - v,
        this->z - v,
        this->w - v);
}

Vec4 Vec4::multiply(float v) const
{
    return Vec4(this->x * v,
        this->y * v,
        this->z * v,
        this->w * v);
}

Vec4 Vec4::divide(float v) const
{
    return Vec4(x / v, y / v, z / v, w / v);
}

float Vec4::length() const
{
    return std::sqrt((x * x) + (y * y) + (z * z) + (w * w));
}

float Vec4::dot(const Vec4 &other) const
{
    return ((this->x * other.x) + (this->z * other.z) + (this->y * other.y) + (this->w * other.w));
}

Vec4 Vec4::normalize() const
{
    return this->divide(length());
}

Vec4 operator+(const Vec4 &left, float v)
{
    return left.add(v);
}

Vec4 operator-(const Vec4 &left, float v)
{
    return left.subtract(v);
}

Vec4 operator*(const Vec4 &left, float v)
{
    return left.multiply(v);
}

Vec4 operator/(const Vec4 &left, float v)
{
    return left.divide(v);
}

float &Vec4::operator[](const std::size_t &v)
{
    return m_Values[v];
}

const float &Vec4::operator[](const std::size_t &v) const
{
    return m_Values[v];
}

Vec4 &Vec4::operator/=(float v)
{
    this->x /= v;
    this->y /= v;
    this->z /= v;
    this->w /= v;
    return *this;
}

bool Vec4::operator!=(const Vec4 &other) const
{
    return !(this->x == other.x && this->y == other.y && this->z == other.z && this->w == other.w);
}

bool Vec4::operator==(const Vec4 &other) const
{
    return this->x == other.x && this->y == other.y && this->z == other.z && this->w == other.w;
}

Vec4 &Vec4::operator*=(float v)
{
    this->x *= v;
    this->y *= v;
    this->z *= v;
    this->w *= v;
    return *this;
}

Vec4 &Vec4::operator-=(const Vec4 &other)
{
    this->x -= other.x;
    this->y -= other.y;
    this->z -= other.z;
    this->w -= other.w;
    return *this;
}

Vec4 &Vec4::operator-=(float v)
{
    this->x -= v;
    this->y -= v;
    this->z -= v;
    this->w -= v;
    return *this;
}

Vec4 &Vec4::operator+=(const Vec4 &other)
{
    this->x += other.x;
    this->y += other.y;
    this->z += other.z;
    this->w += other.w;
    return *this;
}

Vec4 &Vec4::operator+=(float v)
{
    this->x += v;
    this->y += v;
    this->z += v;
    this->w += v;
    return *this;
}

Vec4 operator-(const Vec4 &left, const Vec4 &other)
{
    return left.subtract(other);
}

Vec4 operator+(const Vec4 &left, const Vec4 &other)
{
    return left.add(other);
}

Vec4 Vec4::subtract(const Vec4 &other) const
{
    return Vec4(this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w);
}

Vec4 Vec4::add(const Vec4 &other) const
{
    return Vec4(this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w);
}

Vec4::Vec4(float x, float y, float z, float w)
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{
}
