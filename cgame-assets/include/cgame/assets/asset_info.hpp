#pragma once
#include <cgame/assets/collider_spec.hpp>
#include <string>

namespace cgame::assets
{
    struct asset_info
    {
        std::string assetName;
        std::string modelPath;
        collider_spec colliderSpec;
    };
}