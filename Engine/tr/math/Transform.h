#pragma once

#include "Math.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"

namespace tr {

using Quaternion  = math::quat;
using EulerAngles = Vec3;

struct Transform {
    Vec3       translation{ 0.f };
    Vec3       scale{ 10.f };
    Quaternion rotation;

    Transform operator*(const Transform &other) const
    {
        return Transform{ this->translation + other.translation,
                          this->scale * other.scale,
                          this->rotation * other.rotation };
    }

    explicit operator Mat4() const
    {
        Mat4 m(1.f);
        m = math::translate(m, this->translation);
        m = math::scale(m, this->scale);
        return m * math::mat4_cast(this->rotation);
    }
};
}
