#pragma once

#include <string_view>

namespace cgame::graphics
{
    // Sources for the shaders the module needs to draw anything at all: the
    // default model shader and the debug grid shader.
    //
    // They follow the shader contract expected by the backends:
    //   - uniforms: matView, matProjection (both mat4), and optionally
    //     baseColorTexture (sampler2D, bound to texture slot 0)
    //   - vertex attributes: 0 position (vec3), 1 texcoord (vec2),
    //     2 normal (vec3), 9-12 instance transform (mat4, one column each)
    //
    // A uniform that is not declared simply stays at its default, so the
    // debug grid shader can omit the texture and normal inputs.

    inline constexpr std::string_view defaultVertexShader = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;

layout(location = 9) in mat4 instanceTransform;

uniform mat4 matView;
uniform mat4 matProjection;

out vec2 texCoord;

void main()
{
    vec4 worldPosition = instanceTransform * vec4(vertexPosition, 1.0);
    gl_Position = matProjection * matView * worldPosition;
    texCoord = vertexTexCoord;
}
)";

    inline constexpr std::string_view defaultFragmentShader = R"(
#version 330 core

in vec2 texCoord;

uniform sampler2D baseColorTexture;

out vec4 finalColor;

void main()
{
    finalColor = texture(baseColorTexture, texCoord);
}
)";

    inline constexpr std::string_view debugGridVertexShader = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition;

layout(location = 9) in mat4 instanceTransform;

uniform mat4 matView;
uniform mat4 matProjection;

void main()
{
    vec4 worldPosition = instanceTransform * vec4(vertexPosition, 1.0);
    gl_Position = matProjection * matView * worldPosition;
}
)";

    inline constexpr std::string_view debugGridFragmentShader = R"(
#version 330 core

out vec4 finalColor;

void main()
{
    finalColor = vec4(0.5, 0.5, 0.5, 1.0);
}
)";
}
