#pragma once
#include <bullet/btBulletDynamicsCommon.h>
#include <array>
#include <cstdint>

namespace physics
{
    class collision_shape_pool
    {
    public:
        btCollisionShape *getOrMake(uint8_t colShapeId)
        {
            btCollisionShape *colShape = m_collisionShapes[colShapeId];

            if (colShape == nullptr)
            {
                colShape = m_factories[colShapeId]();
                m_collisionShapes[colShapeId] = colShape;
            }

            return colShape;
        }

        void registerFactory(uint8_t colShapeId, btCollisionShape *(*factory)())
        {
            m_factories[colShapeId] = factory;
        }

        ~collision_shape_pool()
        {
            for (auto *colShape : m_collisionShapes)
                delete colShape;
        }

    private:
        std::array<btCollisionShape *(*)(), UINT8_MAX> m_factories;
        std::array<btCollisionShape *, UINT8_MAX> m_collisionShapes;
    };
}