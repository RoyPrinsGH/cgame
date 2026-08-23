#version 330 core

in vec2 texCoord;

uniform sampler2D baseColorTexture;

out vec4 finalColor;

void main()
{
    finalColor = texture(baseColorTexture, texCoord);
}