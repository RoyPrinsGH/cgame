#pragma once
#include <LinearMath/btVector3.h>
#include <btBulletDynamicsCommon.h>
#include <glm/vec3.hpp>
#include <array>
#include <cstdint>
#include "collision_shape_pool.hpp"
#include <stdexcept>

namespace cgame::physics
{
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

        std::array<btRigidBody *, UINT8_MAX + 1> getActiveRigidBodies()
        {
            return m_rigidBodies;
        }

        [[nodiscard("dropping the rigid body handle loses the ability to despawn it")]]
        rigid_body_handle spawn(uint8_t colShapeId, spawn_data spawnData)
        {
            uint8_t candidateSlotIx = 0;

            // this will hang the engine if more than 256 physics objects are allocated
            while (m_rigidBodies[candidateSlotIx] != nullptr && candidateSlotIx <= UINT8_MAX)
                candidateSlotIx++;

            if (candidateSlotIx > UINT8_MAX)
                throw std::overflow_error("candidateSlotIx");

            auto *colShapePtr = m_collisionShapePoolPtr->getOrMake(colShapeId);

            btScalar mass(spawnData.mass);

            btVector3 localInertia(
                spawnData.inertia.x,
                spawnData.inertia.y,
                spawnData.inertia.z);

            if (mass != 0.f)
                colShapePtr->calculateLocalInertia(mass, localInertia);

            btVector3 spawnPosition(
                spawnData.position.x,
                spawnData.position.y,
                spawnData.position.z);

            btTransform startTransform;
            startTransform.setIdentity();
            startTransform.setOrigin(spawnPosition);

            btDefaultMotionState *myMotionState = new btDefaultMotionState(startTransform);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShapePtr, localInertia);
            auto *rigidBody = new btRigidBody(rbInfo);
            m_dynamicsWorldPtr->addRigidBody(rigidBody);
            m_rigidBodies[candidateSlotIx] = rigidBody;

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
        btDynamicsWorld *m_dynamicsWorldPtr;
        collision_shape_pool *m_collisionShapePoolPtr;
        std::array<btRigidBody *, UINT8_MAX + 1> m_rigidBodies;
    };
}