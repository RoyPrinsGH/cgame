#include <cgame/assets/collider_spec.hpp>
#include <cgame/assets/pak.hpp>
#include <cgame/graphics/camera.hpp>
#include <cgame/graphics/default_shaders.hpp>
#include <cgame/graphics/grid.hpp>
#include <cgame/graphics/model_controller.hpp>
#include <cgame/graphics/render.hpp>
#include <cgame/graphics/render_backend.hpp>
#include <cgame/graphics/render_snapshot.hpp>
#include <cgame/physics/physics_controller.hpp>
#include <cgame/platform/input/button_state_tracker.hpp>
#include <cgame/platform/input/glfw_input_hook.hpp>
#include <cgame/platform/input/glfw_input_stream.hpp>
#include <cgame/platform/window.hpp>

#include <boost/dll/runtime_symbol_info.hpp>
#include <filesystem>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <memory>
#include <span>

#include "events.hpp"
#include "world/gamestate.hpp"

namespace
{
    glm::mat4 shipTransform(const world::ship& ship, float scale)
    {
        return glm::scale(glm::translate(glm::mat4(1.0f), ship.position) *
                              glm::mat4_cast(ship.rotation),
                          glm::vec3(scale));
    }
}

int main(void)
{
    auto* window = cgame::platform::createWindow(1280, 720, "cgame");

    cgame::platform::input::glfw::glfw_raw_input_stream glfwRawInputStream;
    cgame::platform::input::glfw::installGlfwInputHandler(&glfwRawInputStream);

    world::game_state gameState;

    engine::events::tick_history tickHistory;

    cgame::graphics::camera camera;

    camera.position = {0.0f, 5.0f, 5.0f};

    cgame::platform::input::button_state_tracker<> clientOnlyButtonStateTracker;

    auto* physicsController = new cgame::physics::physics_controller();

    auto playerShipHandle = physicsController->spawn(
        cgame::physics::collision_shape::ship, cgame::physics::spawn_data{.mass = 0.5f});

    std::vector<cgame::physics::rigid_body_handle> enemyShipHandles;

    auto executableDir = boost::dll::program_location().parent_path();
    auto pakPath = executableDir / "assets" / "client.cgpak";

    std::filesystem::create_directories(pakPath.parent_path());

    cgame::assets::pak_builder pakBuilder;

    // Dev convenience: pack the client's own assets next to the executable.
    const auto clientAssetsDir =
        executableDir / ".." / ".." / "cgame-client" / "assets";

    pakBuilder.add({{"models", "ships", "baseShip"}}, clientAssetsDir / "ship.glb");

    pakBuilder.build(pakPath);

    cgame::assets::pak pak(pakPath);

    auto backend = cgame::graphics::createGl33Backend(
        [](const char* procName)
        { return reinterpret_cast<void*>(glfwGetProcAddress(procName)); });

    cgame::graphics::model_controller models(backend.get(), &pak);

    const cgame::graphics::shader_handle defaultShader = backend->loadShader(
        cgame::graphics::defaultVertexShader, cgame::graphics::defaultFragmentShader);

    const cgame::graphics::shader_handle gridShader = backend->loadShader(
        cgame::graphics::debugGridVertexShader, cgame::graphics::debugGridFragmentShader);

    const cgame::graphics::model_handle shipModel =
        models.load({{"models", "ships", "baseShip"}});

    cgame::graphics::primitive_data grid = cgame::graphics::makeGrid(1000, 1.0f);
    const cgame::graphics::model_handle gridModel =
        backend->uploadMesh(std::span{&grid, 1}, {});

    bool debugModeEnabled = true;

    int clientTick = 0;
    int syncedTick = 0;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        auto dt = 1.0f / 144.0f;

        clientTick++;

        engine::events::tick_events_unbounded tickEvents;

        // -----==[INPUT PROCESSING]==-----
        while (auto key = glfwRawInputStream.readNextRawNonBlocking())
        {
            if (auto* e =
                    std::get_if<cgame::platform::input::raw::char_key_down>(&key.value()))
            {
                clientOnlyButtonStateTracker.setKeyState(e->key, true);
            }
            else if (auto* e = std::get_if<cgame::platform::input::raw::char_key_up>(
                         &key.value()))
            {
                clientOnlyButtonStateTracker.setKeyState(e->key, false);
            }
            else if (auto* e =
                         std::get_if<cgame::platform::input::raw::mouse_button_down>(
                             &key.value()))
            {
                clientOnlyButtonStateTracker.setMouseButtonState((uint8_t)e->button,
                                                                 true);
            }
            else if (auto* e = std::get_if<cgame::platform::input::raw::mouse_button_up>(
                         &key.value()))
            {
                clientOnlyButtonStateTracker.setMouseButtonState((uint8_t)e->button,
                                                                 false);
            }

            tickEvents.m_inputEvents.push_back(std::move(key.value()));
        }

        // -----==[CAMERA MOVEMENT]==-----
        glm::vec3 cameraPositionDelta{0.0f, 0.0f, 0.0f};

        if (clientOnlyButtonStateTracker.getKeyState(GLFW_KEY_W).first)
            cameraPositionDelta += glm::vec3{0.0f, 0.0f, 0.5f};

        if (clientOnlyButtonStateTracker.getKeyState(GLFW_KEY_S).first)
            cameraPositionDelta += glm::vec3{0.0f, 0.0f, -0.5f};

        if (clientOnlyButtonStateTracker.getKeyState(GLFW_KEY_A).first)
            cameraPositionDelta += glm::vec3{0.5f, 0.0f, 0.0f};

        if (clientOnlyButtonStateTracker.getKeyState(GLFW_KEY_D).first)
            cameraPositionDelta += glm::vec3{-0.5f, 0.0f, 0.0f};

        if (cameraPositionDelta != glm::vec3{0.0f, 0.0f, 0.0f})
            tickEvents.m_cameraEvents.push_back(std::move(
                engine::events::camera::camera_move_event{.delta = cameraPositionDelta}));

        // -----==[RUN EVENTS]==-----
        tickHistory.registerHistory(clientTick, tickEvents);

        for (auto& t : tickHistory.getHistoryFrom(syncedTick))
        {
            printf("-- tick: %i --\n", t.first);

            for (auto& ce : t.second.m_cameraEvents)
            {
                if (auto* c = std::get_if<engine::events::camera::camera_move_event>(&ce))
                {
                    printf("-- camera moved --\n");
                    camera.position += c->delta;
                }
            }

            for (auto& ie : t.second.m_inputEvents)
            {
                if (auto* k =
                        std::get_if<cgame::platform::input::raw::char_key_down>(&ie))
                {
                    if (k->key == GLFW_KEY_K)
                    {
                        printf("-- spawned enemy ship --\n");

                        auto enemyShipHandle = physicsController->spawn(
                            cgame::physics::collision_shape::ship,
                            cgame::physics::spawn_data{
                                .mass = 0.5f,
                                .position = {4.0f, 0.0f, (float)clientTick / 1000.0f}});

                        enemyShipHandles.push_back(enemyShipHandle);
                    }
                }
                else if (auto* k =
                             std::get_if<cgame::platform::input::raw::mouse_button_down>(
                                 &ie))
                {
                    if (k->button == cgame::platform::input::raw::mouse_button::middle)
                    {
                        printf("-- reset camera position --\n");
                        camera.position = {0.0f, 5.0f, 0.0f};
                    }
                }
                else if (auto* k =
                             std::get_if<cgame::platform::input::raw::special_key_down>(
                                 &ie))
                {
                    if (k->key == cgame::platform::input::raw::special_key::f3)
                    {
                        printf("-- toggled debug mode --\n");
                        debugModeEnabled = !debugModeEnabled;
                    }
                }
            }

            syncedTick = t.first;
        }

        physicsController->simulateStep(dt);

        auto physicsSnapshot = physicsController->getSnapshot();
        auto& playerShipSnapshot =
            physicsSnapshot.activeEntities[playerShipHandle.rigidBodyId];
        gameState.m_playerShip.position = playerShipSnapshot.position;
        gameState.m_playerShip.rotation = playerShipSnapshot.rotation;
        gameState.m_enemyShips.clear();
        for (auto enemyShipHandle : enemyShipHandles)
        {
            auto& enemyShipSnapshot =
                physicsSnapshot.activeEntities[enemyShipHandle.rigidBodyId];
            gameState.m_enemyShips.push_back(
                world::ship{.position = enemyShipSnapshot.position,
                            .rotation = enemyShipSnapshot.rotation});
        }

        // -----==[RENDER]==-----
        std::vector<glm::mat4> shipInstances;
        shipInstances.reserve(gameState.m_enemyShips.size() + 1);

        shipInstances.push_back(shipTransform(gameState.m_playerShip, 0.1f));

        for (const auto& ship : gameState.m_enemyShips)
            shipInstances.push_back(shipTransform(ship, 0.1f));

        std::vector<glm::mat4> gridInstances{glm::mat4(1.0f)};

        int fb_width;
        int fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);

        cgame::graphics::render_snapshot snapshot;
        snapshot.camera = camera;
        snapshot.framebufferWidth = fb_width;
        snapshot.framebufferHeight = fb_height;

        if (debugModeEnabled)
            snapshot.entries.push_back({gridShader, gridModel, gridInstances});

        snapshot.entries.push_back({defaultShader, shipModel, shipInstances});

        cgame::graphics::render(*backend, snapshot);

        glfwSwapBuffers(window);
    }

    delete physicsController;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
