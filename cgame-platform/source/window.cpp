#include <cgame/platform/window.hpp>

namespace cgame::platform
{
    GLFWwindow* createWindow(int width, int height, const char* title)
    {
        if (!glfwInit())
            return nullptr;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);

        if (!window)
        {
            glfwTerminate();
            return nullptr;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        return window;
    }
}
