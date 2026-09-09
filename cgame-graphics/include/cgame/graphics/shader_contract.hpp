#pragma once

#include <string_view>

namespace cgame::graphics::shader_contract
{
    // The shader contract between cgame-graphics and every shader written
    // for it, as a single source of truth. Backends look uniforms up by
    // these names; vertex layout locations must match the constants below.
    //
    // Required (load fails if missing):
    //   - matView, matProjection (both mat4)
    //
    // Optional (stays at its default when not declared, so minimal shaders
    // like the debug grid can omit them):
    //   - baseColorTexture (sampler2D, bound to texture slot 0)
    //
    // Vertex attributes: 0 position (vec3), 1 texcoord (vec2),
    // 2 normal (vec3), 9-12 instance transform (mat4, one column each).

    inline constexpr std::string_view viewUniform = "matView";
    inline constexpr std::string_view projectionUniform = "matProjection";
    inline constexpr std::string_view albedoSampler = "baseColorTexture";

    inline constexpr unsigned int positionLocation = 0;
    inline constexpr unsigned int texcoordLocation = 1;
    inline constexpr unsigned int normalLocation = 2;
    inline constexpr unsigned int instanceTransformLocation = 9;
}
