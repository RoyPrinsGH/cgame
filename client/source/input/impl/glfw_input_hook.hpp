#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace engine::input::glfw
{
    class glfw_input_handler
    {
    public:
        virtual void handleGlfwKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods) {};
        virtual void handleGlfwMouseInput(GLFWwindow *window, int button, int action, int mods) {};
    };

    static void InstallGlfwInputHandlerObj(glfw_input_handler *handlerObj)
    {
        auto window = glfwGetCurrentContext();
        glfwSetWindowUserPointer(window, handlerObj);
        glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
                           {    auto *handler = static_cast<glfw_input_handler *>(glfwGetWindowUserPointer(window)); 
                                handler->handleGlfwKeyInput(window, key, scancode, action, mods); });
        glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods)
                                   {    auto *handler = static_cast<glfw_input_handler *>(glfwGetWindowUserPointer(window)); 
                                        handler->handleGlfwMouseInput(window, button, action, mods); });
    }
}