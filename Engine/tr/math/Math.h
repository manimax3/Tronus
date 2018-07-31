#pragma once

#include "../util/NamedType.h"
#include "glm/glm.hpp"

namespace tr {
using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;

using Mat4 = glm::mat4;

using Radians = NamedType<float, struct RadiansType>;
using Unit    = NamedType<float, struct UnitType>;

namespace math = glm;
}
