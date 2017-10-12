#pragma once
#include <tr.h>
#include "Vec4.h"
#include "Vec3.h"

namespace tr
{
	class Quaternion;

	class TR_API Mat4
	{
	public:
		// Constructors
		Mat4();
		Mat4(float diagonal);
		Mat4(float *elements);
		Mat4(const Vec4 &row0, const Vec4 &row1, const Vec4 &row2, const Vec4 &row3);

		// Methods
		Mat4 multiply(const Mat4 &other) const;
		Vec3 multiply(const Vec3 &other) const;
		Vec4 multiply(const Vec4 &other) const;

		Mat4 transpose() const;

		Vec4 get_columm(const std::size_t &col) const;

		// Operator Overloads
		Vec4& operator[](const std::size_t&);
		friend TR_API Mat4 operator*(const Mat4& lhs, const Mat4& rhs);

		// Static Members
		static Mat4 Identity();
		static Mat4 Orthographic(float left, float right, float bottom, float top, float near, float far);
		static Mat4 Perspective(float fov, float aspect_ratio, float near, float far);
		static Mat4 LookAt(const Vec3 &camera, const Vec3 &onject, const Vec3 &up);

		static Mat4 Translation(const Vec3 &translation);
		static Mat4 Rotation(float angle, const Vec3 &axis);
		//static Mat4 Rotation(const Quaternion &quat);
		static Mat4 Scale(const Vec3 &scale);


	public:
		union {
			float m_Values[16];
			Vec4 m_Rows[4];
		};
	};

	TR_API Mat4 operator*(const Mat4& lhs, const Mat4& rhs);

}
