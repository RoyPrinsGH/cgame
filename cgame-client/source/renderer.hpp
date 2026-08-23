#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "camera.hpp"
#include "world/gamestate.hpp"

namespace engine
{
    struct gpu_primitive
    {
        unsigned int vao;
        unsigned int vertex_vbo;
        int vertex_count;
        unsigned int base_color_texture = 0;
    };

    struct gpu_model
    {
        std::vector<gpu_primitive> primitives;
        unsigned instance_vbo;
    };

    class renderer
    {
    public:
        renderer();

        void draw(
            GLFWwindow *window,
            const world::game_state &gameState,
            const camera &camera) const;

        void toggleDebugMode() { m_debugModeEnabled = !m_debugModeEnabled; }

    private:
        bool m_debugModeEnabled = false;
        void drawShip(world::ship ship);
        unsigned int m_defaultShader;
        int m_defaultShaderViewMatrixLocation;
        int m_defaultShaderProjectionMatrixLocation;
        int m_defaultShaderBaseColorTextureLocation;
        gpu_model m_shipModel;
    };
}