#include <memory>
#include <raylib.h>
#include "camera.hpp"
#include "helpers.hpp"
#include "renderer.hpp"
#include "input/impl/glfw_input_hook.hpp"
#include "input/impl/glfw_input_stream.hpp"
#include "input/button_state_tracker.hpp"
#include "world/gamestate.hpp"

int main(void)
{
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(1280, 720, "the high seas -- client");
    // SetTargetFPS(1);

    engine::input::glfw::glfw_raw_input_stream glfwRawInputStream;
    engine::input::glfw::InstallGlfwInputHandlerObj(&glfwRawInputStream);

    world::game_state gameState;

    engine::camera camera;
    engine::renderer renderer;

    camera.setPosition({0.0f, 5.0f, 5.0f});

    engine::input::button_state_tracker<> buttonStateTracker;

    while (!WindowShouldClose())
    {
        while (auto key = glfwRawInputStream.readNextRawNonBlocking())
        {
            if (auto *e = std::get_if<engine::input::raw::char_key_down>(&key.value()))
            {
                buttonStateTracker.setKeyState(e->key, true);
                if (e->key == KEY_K)
                {
                    gameState.m_enemyShips.push_back(world::ship{.m_position = {4.0f, 0.0f, 12.0f}});
                }
            }
            else if (auto *e = std::get_if<engine::input::raw::char_key_up>(&key.value()))
            {
                buttonStateTracker.setKeyState(e->key, false);
            }
            else if (auto *e = std::get_if<engine::input::raw::mouse_button_down>(&key.value()))
            {
                buttonStateTracker.setMouseButtonState((uint8_t)e->button, true);
                if (e->button == engine::input::raw::mouse_button::middle)
                    camera.setPosition({0.0f, 5.0f, 0.0f});
            }
            else if (auto *e = std::get_if<engine::input::raw::mouse_button_up>(&key.value()))
            {
                buttonStateTracker.setMouseButtonState((uint8_t)e->button, false);
            }
        }

        // needs to go to event bus? or keymapper? i dont think we care
        if (buttonStateTracker.getKeyState(KEY_W).first)
        {
            camera.move({0.0f, 0.0f, 0.5f});
        }
        else if (buttonStateTracker.getKeyState(KEY_S).first)
        {
            camera.move({0.0f, 0.0f, -0.5f});
        }

        if (buttonStateTracker.getKeyState(KEY_A).first)
        {
            camera.move({0.5f, 0.0f, 0.0f});
        }
        else if (buttonStateTracker.getKeyState(KEY_D).first)
        {
            camera.move({-0.5f, 0.0f, 0.0f});
        }

        renderer.draw(gameState, camera);
    }

    CloseWindow();
    return 0;
}