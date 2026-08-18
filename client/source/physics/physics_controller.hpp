#pragma once
#include <bullet/btBulletDynamicsCommon.h>
#include "collision_shape_factories/ship_collision_shape_factory.hpp"
#include "collision_shape_pool.hpp"

namespace physics
{
    enum collision_shape
    {
        ship
    };

    class physics_controller
    {
    public:
        physics_controller()
        {
            m_collisionShapePool.registerFactory(
                collision_shape::ship,
                collision_shape_factories::ship_collision_shape_factory::make);

            m_collisionConfiguration = new btDefaultCollisionConfiguration();
            m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
            m_overlappingPairCache = new btDbvtBroadphase();
            m_solver = new btSequentialImpulseConstraintSolver;
            m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);
            m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
        }

        ~physics_controller()
        {
            delete m_dynamicsWorld;
            delete m_solver;
            delete m_overlappingPairCache;
            delete m_dispatcher;
            delete m_collisionConfiguration;
        }

    private:
        collision_shape_pool m_collisionShapePool;

        btDefaultCollisionConfiguration *m_collisionConfiguration;
        btCollisionDispatcher *m_dispatcher;
        btBroadphaseInterface *m_overlappingPairCache;
        btSequentialImpulseConstraintSolver *m_solver;
        btDiscreteDynamicsWorld *m_dynamicsWorld;

        btAlignedObjectArray<btCollisionShape *> m_collisionShapes;
    };
}