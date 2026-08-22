#pragma once
#include "camera.hpp"
#include "raylib.h"
#include "world/gamestate.hpp"

namespace engine
{
    class renderer
    {
    public:
        renderer();
        void draw(const world::game_state &gameState, const camera &camera);
        void toggleDebugMode() { m_debugModeEnabled = !m_debugModeEnabled; }

    private:
        bool m_debugModeEnabled = false;
        Model m_shipModel;
        Camera3D m_raylibCamera;
        void drawShip(world::ship ship, Color tint);
    };
}