#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace engine::input::glfw
{
    class glfw_key_handler
    {
    public:
        virtual void handleGlfwInput(GLFWwindow *window, int key, int scancode, int action, int mods) {};
    };

    static void InstallGlfwKeyHandlerObj(glfw_key_handler *handlerObj)
    {
        auto window = glfwGetCurrentContext();
        glfwSetWindowUserPointer(window, handlerObj);
        glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
                           { auto *handler =
                                 static_cast<glfw_key_handler *>(
                                     glfwGetWindowUserPointer(window)); 
                            handler->handleGlfwInput(window, key, scancode, action, mods); });
    }
}