#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace cgame::platform
{
    // Creates the GLFW window and makes its OpenGL context current.
    // Returns nullptr when GLFW or the window could not be initialised.
    GLFWwindow* createWindow(int width, int height, const char* title);
}
