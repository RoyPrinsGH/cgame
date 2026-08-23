#pragma once
#include <glm/vec3.hpp>

namespace engine
{
    struct camera
    {
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 up = {0.0f, 1.0f, 0.0f};

        float fovY = 45.0f;
        float nearPlane = 0.01f;
        float farPlane = 1000.0f;
    };
}