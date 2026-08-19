#pragma once
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem/path.hpp>
#include "renderer.hpp"

namespace engine
{
    renderer::renderer()
    {
        m_raylibCamera = Camera3D{
            .position = {0.0f, 0.0f, 0.0f},
            .target = {0.0f, 0.0f, 0.0f},
            .up = {0.0f, 1.0f, 0.0f},
            .fovy = 45.0f,
            .projection = CAMERA_PERSPECTIVE,
        };

        m_shipModel = LoadModel(
            boost::dll::program_location()
                .parent_path()
                .append("assets")
                .append("ship.glb")
                .c_str());
    }

    void renderer::draw(const world::game_state &gameState, const camera &camera)
    {
        auto cameraPosition = camera.getPosition();
        auto cameraTarget = camera.getTarget();
        m_raylibCamera.position = {cameraPosition.x, cameraPosition.y, cameraPosition.z};
        m_raylibCamera.target = {cameraTarget.x, cameraTarget.y, cameraTarget.z};
        BeginDrawing();
        ClearBackground(BLACK);
        DrawFPS(0, 0);
        BeginMode3D(m_raylibCamera);
        DrawGrid(100, 0.5f);
        auto glmShipPosition = gameState.m_playerShip.position;
        DrawModel(m_shipModel, {glmShipPosition.x, glmShipPosition.y, glmShipPosition.z}, 0.05f, WHITE);
        for (const auto &enemyShip : gameState.m_enemyShips)
        {
            auto glmEnemyShipPosition = enemyShip.position;
            DrawModel(m_shipModel, {glmEnemyShipPosition.x, glmEnemyShipPosition.y, glmEnemyShipPosition.z}, 0.05f, RED);
        }
        EndMode3D();
        EndDrawing();
    }
}