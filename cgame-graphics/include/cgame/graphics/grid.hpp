#pragma once

#include <cgame/graphics/mesh.hpp>

namespace cgame::graphics
{
    [[nodiscard]]
    primitive_data makeGrid(int slices, float spacing);
}
