#pragma once

// We need a default constructor
#define BETTER_ENUMS_DEFAULT_CONSTRUCTOR(Enum)                                 \
public:                                                                        \
    Enum() = default;
#include "../util/enum.h"

namespace tr {
/**
 * Things in the interface of an Shader
 */
BETTER_ENUM(ShaderElement, int, Uniform, Attribute);

/**
 * The possible types of those ShaderElements
 */
BETTER_ENUM(ShaderElementType,
            int,
            Sampler2D_Albedo,
            Sampler2D_Diffuse,
            Sampler2D_Specular,
            Sampler2D_Other,
            Mat4,
            Vec3,
            Vec2,
            Float,
            Bool,
            Struct);
}
