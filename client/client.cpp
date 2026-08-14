#include <memory>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem/path.hpp>
#include "raylib.h"

int main(void)
{
    boost::filesystem::path assets_path = boost::dll::program_location().parent_path().append("assets");

    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(1280, 720, "the high seas -- client");

    Camera3D camera{
        .position = {0.0f, 5.0f, 5.0f},
        .target = {0.0f, 0.0f, 0.0f},
        .up = {0.0f, 1.0f, 0.0f},
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE,
    };

    Model model = LoadModel(assets_path.append("ship.glb").c_str());

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        DrawFPS(0, 0);

        BeginMode3D(camera);

        DrawGrid(10, 0.5f);
        DrawModel(model, {0.0f, 0.0f, 0.0f}, 0.05f, WHITE);

        EndMode3D();

        EndDrawing();
    }
    CloseWindow();
    return 0;
}