#include <cgame/platform/input/glfw_input_hook.hpp>

namespace cgame::platform::input::glfw
{
    namespace
    {
        GLFWkeyfun PREV_KEY_FUN = nullptr;
        GLFWmousebuttonfun PREV_MOUSE_FUN = nullptr;
    }

    void installGlfwInputHandler(glfw_input_handler* handlerObj)
    {
        GLFWwindow* window = glfwGetCurrentContext();

        glfwSetWindowUserPointer(window, handlerObj);

        PREV_KEY_FUN = glfwSetKeyCallback(
            window,
            [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                if (PREV_KEY_FUN)
                    PREV_KEY_FUN(window, key, scancode, action, mods);

                auto* handler =
                    static_cast<glfw_input_handler*>(glfwGetWindowUserPointer(window));
                handler->handleGlfwKeyInput(window, key, scancode, action, mods);
            });

        PREV_MOUSE_FUN = glfwSetMouseButtonCallback(
            window,
            [](GLFWwindow* window, int button, int action, int mods)
            {
                if (PREV_MOUSE_FUN)
                    PREV_MOUSE_FUN(window, button, action, mods);

                auto* handler =
                    static_cast<glfw_input_handler*>(glfwGetWindowUserPointer(window));
                handler->handleGlfwMouseInput(window, button, action, mods);
            });
    }
}
