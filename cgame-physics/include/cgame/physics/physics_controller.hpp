#pragma once
#include <cgame/physics/rigid_body.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>

namespace cgame::physics
{
    enum collision_shape
    {
        ship
    };

    struct entity_snapshot
    {
        glm::vec3 position;
        glm::vec4 rotation;
    };

    struct physics_snapshot
    {
        std::array<entity_snapshot, UINT8_MAX + 1> activeEntities;
    };

    class physics_controller
    {
    public:
        physics_controller();
        ~physics_controller();
        void simulateStep(float deltaTime);
        rigid_body_handle spawn(collision_shape collisionShape, spawn_data spawnData);
        physics_snapshot getSnapshot();

    private:
        struct impl;
        std::unique_ptr<impl> m_implPtr;
    };
}