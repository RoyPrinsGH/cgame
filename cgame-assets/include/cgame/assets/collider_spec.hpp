#pragma once
#include <glm/vec3.hpp>
#include <string>
#include <variant>
#include <vector>

namespace cgame::assets
{
    struct collider_spec
    {
        struct sphere_spec
        {
            float radius;
        };

        struct box_spec
        {
            float width;
            float height;
            float depth;
        };

        using shape_spec = std::variant<sphere_spec, box_spec>;
        using offset = glm::vec3;

        std::vector<std::pair<shape_spec, offset>> colliderParts;
    };

    struct asset_spec
    {
        std::string name;
        collider_spec colliderSpec;
    };
}