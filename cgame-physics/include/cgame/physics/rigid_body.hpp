#pragma once
#include <cstdint>
#include <glm/vec3.hpp>

namespace cgame::physics
{
    struct rigid_body_handle
    {
        uint8_t rigidBodyId;
    };

    struct spawn_data
    {
        float mass;
        glm::vec3 inertia;
        glm::vec3 position;
    };
}