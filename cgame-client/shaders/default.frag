#version 330 core

in vec3 normal;

out vec4 finalColor;

void main()
{
    vec3 n = normalize(normal);
    finalColor = vec4(abs(n) * 0.7 + 0.3, 1.0);
}