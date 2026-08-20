#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace world
{
    class ship
    {
    public:
        glm::vec3 position;
        glm::vec4 rotation{0.0f, 0.0f, 0.0f, 1.0f};
    };
}