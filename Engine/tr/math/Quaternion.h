#pragma once
#include <tr.h>
#include "Vec3.h"

namespace tr
{
	class TR_API Quaternion
	{
	public:
		Quaternion(float w = 1, float x = 0, float y = 0, float z = 0);
		Quaternion(float scalar, const Vec3 &vector);

		void rotation(float radians, const Vec3 &axis);
		void set(float scalar);
		void set(const Vec3 &vector);

		Quaternion conjuagte()  const;
		Quaternion normalized() const;
		float length()		    const;

		void normalize();

		Quaternion multiply(const Quaternion &other)  const;
		Quaternion multiply(float scalar)			  const;
		Quaternion add	   (const Quaternion &other)  const;
		Quaternion subtract(const Quaternion &other)  const;
		Quaternion divide  (float scalar)			  const;

		// Rotates the vector by the quaternion
		Vec3	   multiply(const Vec3		 &other)  const;

		float	   dot(const Quaternion &other)		  const;

        Quaternion operator*(const Quaternion &other);
        Quaternion operator+(const Quaternion &other);
        Quaternion operator-(const Quaternion &other);
        Vec3 operator*(const Vec3 &other);

		static Quaternion Rotation(float radians, const Vec3 &axis);
		static Quaternion FromEuler(float pitch, float yaw, float roll);

		static Quaternion Interpolate(const Quaternion &start, const Quaternion &end, float t);

	public:
		float w;
        union {
			struct {float x, y, z;};
		    Vec3 vec;
        };
	};
}
