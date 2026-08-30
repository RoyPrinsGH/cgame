#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace cgame::platform::input::glfw
{
    class glfw_input_handler
    {
      public:
        virtual ~glfw_input_handler() = default;

        virtual void handleGlfwKeyInput(GLFWwindow* window,
                                        int key,
                                        int scancode,
                                        int action,
                                        int mods) {};

        virtual void handleGlfwMouseInput(GLFWwindow* window,
                                          int button,
                                          int action,
                                          int mods) {};
    };

    // Chains our callbacks onto any already installed on the current context,
    // so both keep receiving input.
    void installGlfwInputHandler(glfw_input_handler* handlerObj);
}
