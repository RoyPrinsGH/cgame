#pragma once

#include <cstddef>
#include <span>

#include <cgame/graphics/mesh.hpp>

namespace cgame::graphics
{
    model_data loadGltf(std::span<const std::byte> bytes);
}
