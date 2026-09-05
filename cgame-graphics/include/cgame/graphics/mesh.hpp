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

    enum class mesh_topology
    {
        triangles,
        lines
    };

    struct primitive_data
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texcoords;
        std::vector<std::uint32_t> indices;
        int albedoIndex = -1;
        mesh_topology topology = mesh_topology::triangles;
    };

    struct model_data
    {
        std::vector<image_data> images;
        std::vector<primitive_data> primitives;
    };
}
