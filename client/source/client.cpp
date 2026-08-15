#include <memory>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem/path.hpp>
#include "raylib.h"
#include "camera.hpp"
#include "renderer.hpp"
#include "input/impl/glfw_input_hook.hpp"
#include "input/impl/glfw_input_stream.hpp"
#include "helpers.hpp"
#include "input/keyboard_state_tracker.hpp"

int main(void)
{
    boost::filesystem::path assets_path = boost::dll::program_location().parent_path().append("assets");

    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(1280, 720, "the high seas -- client");
    // SetTargetFPS(1);

    engine::input::glfw::glfw_raw_input_stream glfwRawInputStream;
    engine::input::glfw::InstallGlfwKeyHandlerObj(&glfwRawInputStream);

    engine::camera camera;
    engine::renderer renderer;

    camera.setPosition({0.0f, 5.0f, 5.0f});

    Model model = LoadModel(assets_path.append("ship.glb").c_str());

    engine::input::keyboard_state_tracker<std::chrono::steady_clock, UINT8_MAX + 1> keyboardStateTracker;

    while (!WindowShouldClose())
    {
        while (auto key = glfwRawInputStream.readNextRawNonBlocking())
        {
            if (auto *e = std::get_if<engine::input::raw::char_key_down>(&key.value()))
            {
                keyboardStateTracker.setState(e->key, true);
            }
            else if (auto *e = std::get_if<engine::input::raw::char_key_up>(&key.value()))
            {
                keyboardStateTracker.setState(e->key, false);
            }
        }

        // needs to go to event bus? or keymapper? i dont think we care
        if (keyboardStateTracker.getState(KEY_W).first)
        {
            auto cameraPosition = camera.getPosition();
            cameraPosition.z += 0.5f;
            camera.setPosition(cameraPosition);
        }
        else if (keyboardStateTracker.getState(KEY_S).first)
        {
            auto cameraPosition = camera.getPosition();
            cameraPosition.z -= 0.5f;
            camera.setPosition(cameraPosition);
        }

        renderer.setCamera(camera);
        renderer.draw();
    }

    CloseWindow();
    return 0;
}