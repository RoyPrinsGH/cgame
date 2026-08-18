#pragma once
#include <bullet/btBulletDynamicsCommon.h>
#include <array>
#include <cstdint>
#include "collision_shape_pool.hpp"

namespace physics
{
    struct spawn_data
    {
    };

    struct rigid_body_handle
    {
        uint8_t rigidBodyId;
    };

    // takes no ownership of alloc/dealloc of the ptr
    class rigid_body_controller
    {
    public:
        rigid_body_controller(
            btDynamicsWorld *dynamicsWorldPtr,
            collision_shape_pool *collisionShapePoolPtr)
            : m_dynamicsWorldPtr(dynamicsWorldPtr),
              m_collisionShapePoolPtr(collisionShapePoolPtr)
        {
        }

        rigid_body_handle spawn(uint8_t colShapeId, spawn_data spawnData)
        {
            uint8_t candidateSlotIx;

            // this will hang the engine if more than 256 physics objects are allocated
            while (m_rigidBodies[candidateSlotIx] != nullptr)
                candidateSlotIx++;

            auto colShapePtr = m_collisionShapePoolPtr->getOrMake(colShapeId);

            // TODO: btRigidBody building based on spawn_data
            m_rigidBodies[candidateSlotIx] = new btRigidBody(colShapePtr);

            return rigid_body_handle{.rigidBodyId = candidateSlotIx};
        }

        void despawn(rigid_body_handle rigidBodyHandle)
        {
            auto rigidBodyPtr = m_rigidBodies[rigidBodyHandle.rigidBodyId];
            if (rigidBodyPtr == nullptr)
                return;

            m_dynamicsWorldPtr->removeRigidBody(rigidBodyPtr);
            delete rigidBodyPtr->getMotionState();
            delete rigidBodyPtr;

            m_rigidBodies[rigidBodyHandle.rigidBodyId] = nullptr;
        }

    private:
        collision_shape_pool *m_collisionShapePoolPtr;
        btDynamicsWorld *m_dynamicsWorldPtr;
        std::array<btRigidBody *, UINT8_MAX + 1> m_rigidBodies;
    };
}