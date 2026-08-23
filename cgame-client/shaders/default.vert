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