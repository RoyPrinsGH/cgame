#pragma once
#include <btBulletDynamicsCommon.h>

namespace physics::collision_shape_factories
{
    class ship_collision_shape_factory
    {
    public:
        static btCollisionShape *make()
        {
            return new btSphereShape(btScalar(1.));
        }
    };
}