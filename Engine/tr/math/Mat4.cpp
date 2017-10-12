#include "Mat4.h"

using namespace tr;

Mat4::Mat4(float diagonal)
{
	std::memset(m_Values, 0, 16 * sizeof(float));
	m_Values[0 + 0 * 4] = diagonal;
	m_Values[1 + 1 * 4] = diagonal;
	m_Values[2 + 2 * 4] = diagonal;
	m_Values[3 + 3 * 4] = diagonal;
}

Mat4::Mat4()
{
	std::memset(m_Values, 0, 16 * sizeof(float));
}

Mat4 Mat4::multiply(const Mat4 &other) const
{
	Mat4 mat;
	for(std::size_t i = 0; i < 4; i++)
		for (std::size_t j = 0; j < 4; j++)
		{
			const Vec4 &row = m_Rows[i];
			const Vec4 &col = other.get_columm(j);
			float dot = row.dot(col);
			mat[i][j] = dot;
		}
	return mat;
}

Mat4 Mat4::transpose() const
{
	return Mat4(get_columm(0), get_columm(1), get_columm(2), get_columm(3));
}

Vec4 Mat4::get_columm(const std::size_t &col) const
{
	return Vec4(m_Rows[0][col], m_Rows[1][col], m_Rows[2][col], m_Rows[3][col]);
}

Vec4& Mat4::operator[](const std::size_t& v)
{
	return m_Rows[v];
}

Mat4 Mat4::Identity()
{
	return Mat4(1.0f);
}

Mat4 Mat4::Orthographic(float left, float right, float bottom, float top, float near, float far)
{
	Mat4 result(1.0f);

	result.m_Values[0 + 0 * 4] = 2.0f / (right - left);
	result.m_Values[1 + 1 * 4] = 2.0f / (top - bottom);
	result.m_Values[2 + 2 * 4] = 2.0f / (near - far);
	result.m_Values[3 + 0 * 4] = (left + right) / (left - right);
	result.m_Values[3 + 1 * 4] = (bottom + top) / (bottom - top);
	result.m_Values[3 + 2 * 4] = (far + near) / (far - near);

	return result;
}

Mat4 Mat4::Perspective(float fov, float aspect_ratio, float near, float far)
{
	Mat4 result(1.0f);

	float q = 1.0f / std::tan(0.5f * fov);
	float a = q / aspect_ratio;

	float b = (near + far) / (near - far);
	float c = (2.0f * near * far) / (near - far);

	result.m_Values[0 + 0 * 4] = a;
	result.m_Values[1 + 1 * 4] = q;
	result.m_Values[2 + 2 * 4] = b;
	result.m_Values[2 + 3 * 4] = -1.0f;
	result.m_Values[3 + 2 * 4] = c;

	return result;
}

Mat4 Mat4::LookAt(const Vec3 &camera, const Vec3 &object, const Vec3 &up)
{
	Mat4 result = Identity();
	Vec3 f = (object - camera).normalize();
	Vec3 s = f.cross(up.normalize());
	Vec3 u = s.cross(f);

	result.m_Values[0 + 0 * 4] = s.x;
	result.m_Values[0 + 1 * 4] = s.y;
	result.m_Values[0 + 2 * 4] = s.z;

	result.m_Values[1 + 0 * 4] = u.x;
	result.m_Values[1 + 1 * 4] = u.y;
	result.m_Values[1 + 2 * 4] = u.z;

	result.m_Values[2 + 0 * 4] = -f.x;
	result.m_Values[2 + 1 * 4] = -f.y;
	result.m_Values[2 + 2 * 4] = -f.z;

	return result * Translation(Vec3(-camera.x, -camera.y, -camera.z));
}

Mat4 Mat4::Translation(const Vec3 &translation)
{
	return Mat4(Vec4(0, 0, 0, translation.x), Vec4(0, 0, 0, translation.y), Vec4(0, 0, 0, translation.z), Vec4(0, 0, 0, 0));
}

Mat4 Mat4::Rotation(float angle, const Vec3 &_axis)
{
	auto axis = _axis.normalize();
	float c = std::cos(angle);
	float c1 = 1 - c;
	float s = std::sin(angle);

	return Mat4(Vec4(c + (axis.x * axis.x) * c1, axis.x * axis.y * c1 - axis.z * s, axis.x * axis.z * c1 + axis.y * s, 0),
		Vec4(axis.y * axis.z * c1 + axis.z * s, c + axis.y * axis.y * c1, axis.y * axis.z * c1 - axis.x * s, 0),
		Vec4(axis.z * axis.x * c1 - axis.y * s, axis.z * axis.y * c1 + axis.x * s, c + axis.z * axis.z * c1, 0),
		Vec4(0, 0, 0, 1));

}

Mat4 Mat4::Scale(const Vec3 &scale)
{
	return Mat4(Vec4(scale.x, 0, 0, 0), Vec4(0, scale.y, 0, 0), Vec4(0, 0, scale.z, 0), Vec4(0, 0, 0, 0));
}

Vec4 Mat4::multiply(const Vec4 &other) const
{
	Vec4 result;
	result.x = this->m_Rows[0].dot(other);
	result.y = this->m_Rows[1].dot(other);
	result.z = this->m_Rows[2].dot(other);
	result.w = this->m_Rows[3].dot(other);
	return result;
}

Vec3 Mat4::multiply(const Vec3 &other) const
{
	Vec3 result;
	result.x = this->m_Rows[0].dot(other);
	result.y = this->m_Rows[1].dot(other);
	result.z = this->m_Rows[2].dot(other);
	return result;
}

Mat4::Mat4(const Vec4 &row0, const Vec4 &row1, const Vec4 &row2, const Vec4 &row3)
{
	m_Rows[0] = row0;
	m_Rows[1] = row1;
	m_Rows[2] = row2;
	m_Rows[3] = row3;
}

Mat4::Mat4(float *elements)
{
	std::memcpy(m_Values, elements, 16 * sizeof(float));
}

Mat4 tr::operator*(const Mat4& lhs, const Mat4& rhs)
{
	return lhs.multiply(rhs);
}