#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace world
{
    class ship
    {
    public:
        glm::vec3 position;
        glm::quat rotation{0.0f, 0.0f, 0.0f, 1.0f};
    };
}