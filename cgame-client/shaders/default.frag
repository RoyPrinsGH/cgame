#version 330 core

in vec2 texCoord;

uniform sampler2D baseColorTexture;

out vec4 finalColor;

void main()
{
    vec2 uv = fract(texCoord);
    finalColor = vec4(uv.x, uv.y, 0.0, 1.0);
}