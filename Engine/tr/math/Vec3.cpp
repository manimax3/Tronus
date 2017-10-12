#include "Vec3.h"

using namespace tr;

Vec3::Vec3(float v)
	: x(v), y(v), z(v)
{
}

Vec3 Vec3::add(float v) const
{
	return Vec3(this->x + v,
		this->y + v,
		this->z + v);
}

Vec3 Vec3::subtract(float v) const
{
	return Vec3(this->x - v,
		this->y - v,
		this->z - v);
}

Vec3 Vec3::multiply(float v) const
{
	return Vec3(this->x * v,
		this->y * v,
		this->z * v);
}

Vec3 Vec3::divide(float v) const
{
	return Vec3(x / v, y / v, z / v);
}

float Vec3::length() const
{
	return std::sqrt((x*x) + (y*y) + (z*z));
}

float Vec3::dot(const Vec3 &other) const
{
	return ((this->x * other.x) + (this->z * other.z) + (this->y * other.y));
}

Vec3 Vec3::cross(const Vec3 &other) const
{
	return Vec3((this->y * other.z) - (this->z * other.y),
		(this->z * other.x) - (this->x * other.z),
		(this->x * other.y) - (this->y * other.x));
}

Vec3 Vec3::normalize() const
{
	return *this / length();
}

Vec3 operator+(const Vec3 &left, float v)
{
	return left.add(v);
}

Vec3 operator-(const Vec3 &left, float v)
{
	return left.subtract(v);
}

Vec3 operator*(const Vec3 &left, float v)
{
	return left.multiply(v);
}

Vec3 operator/(const Vec3 &left, float v)
{
	return left.divide(v);
}

float& Vec3::operator[](const std::size_t& v)
{
	return m_Values[v];
}


Vec3& Vec3::operator/=(float v)
{
	this->x /= v;
	this->y /= v;
	this->z /= v;
	return *this;
}

bool Vec3::operator!=(const Vec3& other) const
{
	return !(this->x == other.x && this->y == other.y && this->z == other.z);
}

bool Vec3::operator==(const Vec3& other) const
{
	return this->x == other.x && this->y == other.y && this->z == other.z;
}

Vec3& Vec3::operator*=(float v)
{
	this->x *= v;
	this->y *= v;
	this->z *= v;
	return *this;
}

Vec3& Vec3::operator-=(const Vec3 &other)
{
	this->x -= other.x;
	this->y -= other.y;
	this->z -= other.z;
	return *this;
}

Vec3& Vec3::operator-=(float v)
{
	this->x -= v;
	this->y -= v;
	this->z -= v;
	return *this;
}

Vec3& Vec3::operator+=(const Vec3 &other)
{
	this->x += other.x;
	this->y += other.y;
	this->z += other.z;
	return *this;
}

Vec3& Vec3::operator+=(float v)
{
	this->x += v;
	this->y += v;
	this->z += v;
	return *this;
}

Vec3 tr::operator-(const Vec3 &left, const Vec3 &right)
{
	return left.subtract(right);
}

Vec3 tr::operator+(const Vec3 &left, const Vec3 &right)
{
	return left.add(right);
}

Vec3 Vec3::subtract(const Vec3 &other) const
{
	return Vec3(this->x - other.x, this->y - other.y, this->z - other.z);
}

Vec3 Vec3::add(const Vec3 &other) const
{
	return Vec3(this->x + other.x, this->y + other.y, this->z + other.z);
}

Vec3::Vec3(float x, float y, float z)
	: x(x), y(y), z(z)
{
}
