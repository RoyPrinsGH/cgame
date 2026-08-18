#pragma once
#include <bullet/btBulletDynamicsCommon.h>
#include <array>
#include <cstdint>

namespace physics
{
    struct rigid_body_handle
    {
        uint8_t rigidBodyId;
    };

    // takes no ownership of alloc/dealloc of the ptr
    class rigid_body_mapper
    {
    public:
        rigid_body_handle makeHandleFor(btRigidBody *rigidBodyPtr)
        {
            m_rigidBodies[m_availableIndex] = rigidBodyPtr;
            return rigid_body_handle{.rigidBodyId = m_availableIndex++};
        }

        btRigidBody *getFromHandle(rigid_body_handle rigidBodyHandle)
        {
            m_rigidBodies[rigidBodyHandle.rigidBodyId];
        }

    private:
        uint8_t m_availableIndex = 0;
        std::array<btRigidBody *, UINT8_MAX> m_rigidBodies;
    };
}