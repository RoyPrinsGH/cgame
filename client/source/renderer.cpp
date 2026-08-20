#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem/path.hpp>
#include "renderer.hpp"
#include <raymath.h>

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
        drawShip(gameState.m_playerShip, WHITE);
        for (const auto &enemyShip : gameState.m_enemyShips)
        {
            drawShip(enemyShip, RED);
        }
        EndMode3D();
        EndDrawing();
    }

    void renderer::drawShip(world::ship ship, Color tint)
    {
        Model instance = m_shipModel;
        auto shipRotation = ship.rotation;
        Quaternion raylibQuat = {shipRotation.x, shipRotation.y, shipRotation.z, shipRotation.w};
        instance.transform = QuaternionToMatrix(raylibQuat);
        auto glmShipPosition = ship.position;
        DrawModel(instance, {glmShipPosition.x, glmShipPosition.y, glmShipPosition.z}, 0.05f, WHITE);
    }
}