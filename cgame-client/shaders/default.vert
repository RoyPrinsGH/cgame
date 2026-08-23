#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 2) in vec3 vertexNormal;

layout(location = 9) in mat4 instanceTransform;

uniform mat4 matView;
uniform mat4 matProjection;

out vec3 normal;

void main()
{
    vec4 worldPosition = instanceTransform * vec4(vertexPosition, 1.0);
    gl_Position = matProjection * matView * worldPosition;
    normal = mat3(instanceTransform) * vertexNormal;
}