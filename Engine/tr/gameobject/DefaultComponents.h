#pragma once
#include <tr.h>

#include "../math/Math.h"

#include <string>

namespace tr {
struct SceneComponent {

    explicit SceneComponent(Vec3      position  = Vec3{ 0.f },
                            glm::mat3 transform = glm::mat3(1.f)) noexcept
        : position(position)
        , transform(transform)
    {
    }

    Vec3      position;
    glm::mat3 transform;
};

struct NameComponent {
    explicit NameComponent(const std::string &name = "NONAME")
        : name(name)
    {
    }

    std::string name;
};

};
