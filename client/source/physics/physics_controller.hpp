#pragma once
#include <bullet/btBulletDynamicsCommon.h>
#include "collision_shape_factories/ship_collision_shape_factory.hpp"
#include "collision_shape_pool.hpp"
#include "rigid_body_controller.hpp"

namespace physics
{
    enum collision_shape
    {
        ship
    };

    struct entity_snapshot
    {
        glm::vec3 position;
    };

    struct physics_snapshot
    {
        std::array<entity_snapshot, UINT8_MAX + 1> activeEntities;
    };

    class physics_controller
    {
    public:
        physics_controller()
        {
            m_collisionShapePool = new collision_shape_pool();
            m_collisionShapePool->registerFactory(
                collision_shape::ship,
                collision_shape_factories::ship_collision_shape_factory::make);

            m_collisionConfiguration = new btDefaultCollisionConfiguration();
            m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
            m_overlappingPairCache = new btDbvtBroadphase();
            m_solver = new btSequentialImpulseConstraintSolver;
            m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);
            m_dynamicsWorld->setGravity(btVector3(0, -10, 0));

            m_rigidBodyController = new rigid_body_controller(m_dynamicsWorld, m_collisionShapePool);
        }

        rigid_body_handle spawn(collision_shape collisionShape, spawn_data spawnData)
        {
            return m_rigidBodyController->spawn(collisionShape, spawnData);
        }

        physics_snapshot getSnapshot()
        {
            physics_snapshot physicsSnapshot;

            auto activeBodies = m_rigidBodyController->getActiveRigidBodies();

            for (int i = 0; i <= UINT8_MAX; i++)
            {
                auto *activeBody = activeBodies[i];
                if (activeBody == nullptr)
                    continue;

                btTransform trans;
                activeBody->getMotionState()->getWorldTransform(trans);
                auto physicsEngineNativePosition = trans.getOrigin();

                physicsSnapshot.activeEntities[i] = entity_snapshot{
                    .position = {
                        physicsEngineNativePosition.getX(),
                        physicsEngineNativePosition.getY(),
                        physicsEngineNativePosition.getZ()}};
            }

            return physicsSnapshot;
        }

        ~physics_controller()
        {
            delete m_rigidBodyController;
            delete m_dynamicsWorld;
            delete m_solver;
            delete m_overlappingPairCache;
            delete m_dispatcher;
            delete m_collisionConfiguration;
            delete m_collisionShapePool;
        }

    private:
        collision_shape_pool *m_collisionShapePool;
        rigid_body_controller *m_rigidBodyController;

        btDefaultCollisionConfiguration *m_collisionConfiguration;
        btCollisionDispatcher *m_dispatcher;
        btBroadphaseInterface *m_overlappingPairCache;
        btSequentialImpulseConstraintSolver *m_solver;
        btDiscreteDynamicsWorld *m_dynamicsWorld;
    };
}