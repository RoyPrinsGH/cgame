#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace engine::input::glfw
{
    static GLFWkeyfun RAYLIB_KEY_FUN = nullptr;
    static GLFWmousebuttonfun RAYLIB_MOUSE_FUN = nullptr;

    class glfw_input_handler
    {
    public:
        virtual void handleGlfwKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods) {};
        virtual void handleGlfwMouseInput(GLFWwindow *window, int button, int action, int mods) {};
    };

    static void InstallGlfwInputHandlerObj(glfw_input_handler *handlerObj)
    {
        GLFWwindow *window = glfwGetCurrentContext();

        glfwSetWindowUserPointer(window, handlerObj);

        RAYLIB_KEY_FUN =
            glfwSetKeyCallback(
                window,
                [](GLFWwindow *window,
                   int key,
                   int scancode,
                   int action,
                   int mods)
                {
                    if (RAYLIB_KEY_FUN)
                        RAYLIB_KEY_FUN(window, key, scancode, action, mods);
                    auto *handler = static_cast<glfw_input_handler *>(glfwGetWindowUserPointer(window));
                    handler->handleGlfwKeyInput(window, key, scancode, action, mods);
                });

        RAYLIB_MOUSE_FUN =
            glfwSetMouseButtonCallback(
                window,
                [](GLFWwindow *window,
                   int button,
                   int action,
                   int mods)
                {
                    if (RAYLIB_MOUSE_FUN)
                        RAYLIB_MOUSE_FUN(window, button, action, mods);
                    auto *handler = static_cast<glfw_input_handler *>(glfwGetWindowUserPointer(window));
                    handler->handleGlfwMouseInput(window, button, action, mods);
                });
    }
}