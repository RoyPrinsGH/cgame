#include <memory>
#include <raylib.h>
#include "camera.hpp"
#include "helpers.hpp"
#include "renderer.hpp"
#include "input/impl/glfw_input_hook.hpp"
#include "input/impl/glfw_input_stream.hpp"
#include "input/button_state_tracker.hpp"
#include "world/gamestate.hpp"
#include "events.hpp"

int main(void)
{
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(1280, 720, "the high seas -- client");
    // SetTargetFPS(1);

    engine::input::glfw::glfw_raw_input_stream glfwRawInputStream;
    engine::input::glfw::InstallGlfwInputHandlerObj(&glfwRawInputStream);

    world::game_state gameState;

    engine::events::tick_history tickHistory;

    engine::camera camera;
    engine::renderer renderer;

    camera.setPosition({0.0f, 5.0f, 5.0f});

    engine::input::button_state_tracker<> clientOnlyButtonStateTracker;

    int clientTick;
    int syncedTick;

    while (!WindowShouldClose())
    {
        clientTick++;

        engine::events::tick_events_unbounded tickEvents;

        // -----==[INPUT PROCESSING]==-----
        while (auto key = glfwRawInputStream.readNextRawNonBlocking())
        {
            if (auto *e = std::get_if<engine::input::raw::char_key_down>(&key.value()))
            {
                clientOnlyButtonStateTracker.setKeyState(e->key, true);
            }
            else if (auto *e = std::get_if<engine::input::raw::char_key_up>(&key.value()))
            {
                clientOnlyButtonStateTracker.setKeyState(e->key, false);
            }
            else if (auto *e = std::get_if<engine::input::raw::mouse_button_down>(&key.value()))
            {
                clientOnlyButtonStateTracker.setMouseButtonState((uint8_t)e->button, true);
            }
            else if (auto *e = std::get_if<engine::input::raw::mouse_button_up>(&key.value()))
            {
                clientOnlyButtonStateTracker.setMouseButtonState((uint8_t)e->button, false);
            }

            tickEvents.m_inputEvents.push_back(std::move(key.value()));
        }

        // -----==[CAMERA MOVEMENT]==-----
        glm::vec3 cameraPositionDelta{0.0f, 0.0f, 0.0f};

        if (clientOnlyButtonStateTracker.getKeyState(KEY_W).first)
            cameraPositionDelta += glm::vec3{0.0f, 0.0f, 0.5f};

        if (clientOnlyButtonStateTracker.getKeyState(KEY_S).first)
            cameraPositionDelta += glm::vec3{0.0f, 0.0f, -0.5f};

        if (clientOnlyButtonStateTracker.getKeyState(KEY_A).first)
            cameraPositionDelta += glm::vec3{0.5f, 0.0f, 0.0f};

        if (clientOnlyButtonStateTracker.getKeyState(KEY_D).first)
            cameraPositionDelta += glm::vec3{-0.5f, 0.0f, 0.0f};

        tickEvents.m_cameraEvents.push_back(std::move(engine::events::camera::camera_move_event{.delta = cameraPositionDelta}));

        // -----==[RUN EVENTS]==-----
        tickHistory.registerHistory(clientTick, tickEvents);

        for (auto &t : tickHistory.getHistoryFrom(syncedTick))
        {
            printf("tick: %i", t.first);

            for (auto &ce : t.second.m_cameraEvents)
            {
                if (auto *c = std::get_if<engine::events::camera::camera_move_event>(&ce))
                {
                    camera.move(c->delta);
                }
            }

            for (auto &ie : t.second.m_inputEvents)
            {
                if (auto *k = std::get_if<engine::input::raw::char_key_down>(&ie))
                {
                    if (k->key == KEY_K)
                        gameState.m_enemyShips.push_back(std::move(world::ship{.m_position = {4.0f, 0.0f, 12.0f}}));
                }
                else if (auto *k = std::get_if<engine::input::raw::mouse_button_down>(&ie))
                {
                    if (k->button == raw::mouse_button::middle)
                        camera.setPosition({0.0f, 5.0f, 0.0f});
                }
            }

            syncedTick = t.first;
        }

        // -----==[RENDER]==-----
        renderer.draw(gameState, camera);
    }

    CloseWindow();
    return 0;
}