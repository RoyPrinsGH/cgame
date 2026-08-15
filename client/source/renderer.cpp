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
    }

    void renderer::setCamera(const engine::camera &camera)
    {
        auto cameraPosition = camera.getPosition();
        auto cameraTarget = camera.getTarget();
        m_raylibCamera.position = {cameraPosition.x, cameraPosition.y, cameraPosition.z};
        m_raylibCamera.target = {cameraTarget.x, cameraTarget.y, cameraTarget.z};
    }

    void renderer::draw() const
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawFPS(0, 0);
        BeginMode3D(m_raylibCamera);
        DrawGrid(10, 0.5f);
        EndMode3D();
        EndDrawing();
    }
}