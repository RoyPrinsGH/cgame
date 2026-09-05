#include <cstdint>
#include <numeric>

#include <cgame/graphics/grid.hpp>

namespace cgame::graphics
{
    primitive_data makeGrid(int slices, float spacing)
    {
        const int half = slices / 2;
        const float extent = half * spacing;

        primitive_data grid;
        grid.topology = mesh_topology::lines;

        for (int i = -half; i <= half; ++i)
        {
            const float offset = i * spacing;

            grid.positions.push_back({offset, 0.0f, -extent});
            grid.positions.push_back({offset, 0.0f, extent});
            grid.positions.push_back({-extent, 0.0f, offset});
            grid.positions.push_back({extent, 0.0f, offset});
        }

        grid.normals.assign(grid.positions.size(), glm::vec3(0.0f, 1.0f, 0.0f));
        grid.texcoords.assign(grid.positions.size(), glm::vec2(0.0f));

        grid.indices.resize(grid.positions.size());
        std::iota(grid.indices.begin(), grid.indices.end(), std::uint32_t{0});

        return grid;
    }
}
