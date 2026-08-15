#pragma once
#include "camera.hpp"
#include "glm/vec3.hpp"
#include "raylib.h"

namespace engine
{
    class renderer
    {
    public:
        renderer();
        void setCamera(const engine::camera &camera);
        void draw() const;

    private:
        Camera3D m_raylibCamera;
    };
}