#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "glfw_input_hook.hpp"
#include "../raw_input_stream.hpp"
#include "../../ring_buffer.hpp"

namespace engine::input::glfw
{
    struct glfw_key_press
    {
        int key;
        int action;
    };

    class glfw_raw_input_stream : public raw_input_stream, public glfw_key_handler
    {
    public:
        virtual const std::optional<raw::input_event> readNextRawNonBlocking() override;
        virtual void handleGlfwInput(GLFWwindow *window, int key, int scancode, int action, int mods) override
        {
            m_eventQueue.push(glfw_key_press{.key = key, .action = action});
        }

    private:
        SpscQueue<glfw_key_press, 1024> m_eventQueue;
    };
}