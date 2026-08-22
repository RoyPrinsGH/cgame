#pragma once
#include <cgame/assets/collider_spec.hpp>

namespace cgame::assets
{
    class asset_loader
    {
    public:
        collider_spec getColliderSpec();
    };
}