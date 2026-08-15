#include <memory>
#include <raylib.h>
#include "camera.hpp"
#include "helpers.hpp"
#include "renderer.hpp"
#include "input/impl/glfw_input_hook.hpp"
#include "input/impl/glfw_input_stream.hpp"
#include "input/keyboard_state_tracker.hpp"
#include "world/gamestate.hpp"

int main(void)
{
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(1280, 720, "the high seas -- client");
    // SetTargetFPS(1);

    engine::input::glfw::glfw_raw_input_stream glfwRawInputStream;
    engine::input::glfw::InstallGlfwKeyHandlerObj(&glfwRawInputStream);

    world::game_state gameState;

    engine::camera camera;
    engine::renderer renderer;

    camera.setPosition({0.0f, 5.0f, 5.0f});

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
            camera.move({0.0f, 0.0f, 0.5f});
        }
        else if (keyboardStateTracker.getState(KEY_S).first)
        {
            camera.move({0.0f, 0.0f, -0.5f});
        }

        if (keyboardStateTracker.getState(KEY_A).first)
        {
            camera.move({0.5f, 0.0f, 0.0f});
        }
        else if (keyboardStateTracker.getState(KEY_D).first)
        {
            camera.move({-0.5f, 0.0f, 0.0f});
        }

        renderer.draw(gameState, camera);
    }

    CloseWindow();
    return 0;
}