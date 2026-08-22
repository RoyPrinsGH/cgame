#include <btBulletDynamicsCommon.h>
#include <cgame/physics/physics_controller.hpp>

#include "collision_shape_pool.hpp"
#include "rigid_body_controller.hpp"
#include "ship_collision_shape_factory.hpp"

namespace cgame::physics
{
    struct physics_controller::impl
    {
        collision_shape_pool *collisionShapePool;
        rigid_body_controller *rigidBodyController;

        btDefaultCollisionConfiguration *collisionConfiguration;
        btCollisionDispatcher *dispatcher;
        btBroadphaseInterface *overlappingPairCache;
        btSequentialImpulseConstraintSolver *solver;
        btDiscreteDynamicsWorld *dynamicsWorld;
    };

    physics_controller::physics_controller()
    {
        m_implPtr = std::make_unique<impl>();
        m_implPtr->collisionShapePool = new collision_shape_pool();
        m_implPtr->collisionShapePool->registerFactory(
            collision_shape::ship,
            collision_shape_factories::ship_collision_shape_factory::make);

        m_implPtr->collisionConfiguration = new btDefaultCollisionConfiguration();
        m_implPtr->dispatcher = new btCollisionDispatcher(m_implPtr->collisionConfiguration);
        m_implPtr->overlappingPairCache = new btDbvtBroadphase();
        m_implPtr->solver = new btSequentialImpulseConstraintSolver;

        m_implPtr->dynamicsWorld = new btDiscreteDynamicsWorld(
            m_implPtr->dispatcher,
            m_implPtr->overlappingPairCache,
            m_implPtr->solver,
            m_implPtr->collisionConfiguration);

        m_implPtr->dynamicsWorld->setGravity(btVector3(0, -10, 0));

        m_implPtr->rigidBodyController = new rigid_body_controller(
            m_implPtr->dynamicsWorld,
            m_implPtr->collisionShapePool);
    }

    void physics_controller::simulateStep(float deltaTime)
    {
        auto activeBodies = m_implPtr->rigidBodyController->getActiveRigidBodies();

        for (int i = 0; i <= UINT8_MAX; i++)
        {
            auto *activeBody = activeBodies[i];
            if (activeBody == nullptr)
                continue;

            activeBody->activate(true);

            const float waterY = 0.0f;
            const float gravity = 10.0f;
            const float halfHeight = 0.5f;

            float y = activeBody->getWorldTransform().getOrigin().getY();

            float bottom = y - halfHeight;
            float top = y + halfHeight;

            float submergedFraction = 0.0f;

            if (top <= waterY)
            {
                submergedFraction = 1.0f;
            }
            else if (bottom < waterY)
            {
                submergedFraction =
                    (waterY - bottom) / (top - bottom);
            }

            if (submergedFraction <= 0.0f)
                continue;

            float mass = activeBody->getMass();

            float buoyancyStrength = 1.3f;

            float buoyancyForce =
                mass *
                gravity *
                buoyancyStrength *
                submergedFraction;

            activeBody->applyCentralForce({0.0f, buoyancyForce, 0.0f});

            btVector3 velocity = activeBody->getLinearVelocity();

            float waterDrag = 0.04f;

            btVector3 waterDragForce =
                -velocity * waterDrag * submergedFraction;

            activeBody->applyCentralForce(waterDragForce);
        }

        m_implPtr->dynamicsWorld->stepSimulation(deltaTime, 10);
    }

    rigid_body_handle physics_controller::spawn(collision_shape collisionShape, spawn_data spawnData)
    {
        return m_implPtr->rigidBodyController->spawn(collisionShape, spawnData);
    }

    physics_snapshot physics_controller::getSnapshot()
    {
        physics_snapshot physicsSnapshot;

        auto activeBodies = m_implPtr->rigidBodyController->getActiveRigidBodies();

        for (int i = 0; i <= UINT8_MAX; i++)
        {
            auto *activeBody = activeBodies[i];
            if (activeBody == nullptr)
                continue;

            btTransform trans;
            activeBody->getMotionState()->getWorldTransform(trans);
            auto physicsEngineNativePosition = trans.getOrigin();
            auto physicsEngineNativeRotation = trans.getRotation();

            physicsSnapshot.activeEntities[i] = entity_snapshot{
                .position = {physicsEngineNativePosition.getX(), physicsEngineNativePosition.getY(), physicsEngineNativePosition.getZ()},
                .rotation = {physicsEngineNativeRotation.getX(), physicsEngineNativeRotation.getY(), physicsEngineNativeRotation.getZ(), physicsEngineNativeRotation.getW()}};
        }

        return physicsSnapshot;
    }

    physics_controller::~physics_controller()
    {
        delete m_implPtr->rigidBodyController;
        delete m_implPtr->dynamicsWorld;
        delete m_implPtr->solver;
        delete m_implPtr->overlappingPairCache;
        delete m_implPtr->dispatcher;
        delete m_implPtr->collisionConfiguration;
        delete m_implPtr->collisionShapePool;
    }
}