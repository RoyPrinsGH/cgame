#pragma once

#include <cstdint>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace cgame::graphics
{
    struct image_data
    {
        int width = 0;
        int height = 0;
        int channels = 0;
        std::vector<std::uint8_t> pixels;
    };

    struct mesh_data
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texcoords;
        std::vector<std::uint16_t> indices;
        image_data albedo;
    };

    [[nodiscard]]
    mesh_data makeGridMesh(int slices, float spacing);
}
