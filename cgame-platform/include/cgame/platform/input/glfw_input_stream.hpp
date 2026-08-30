#pragma once

#include <boost/lockfree/spsc_queue.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cgame/platform/input/glfw_input_hook.hpp>
#include <cgame/platform/input/raw_input_stream.hpp>

namespace cgame::platform::input::glfw
{
    struct glfw_key_input
    {
        int key;
        int action;
    };

    struct glfw_mouse_input
    {
        int button;
        int action;
    };

    class glfw_raw_input_stream : public raw_input_stream, public glfw_input_handler
    {
      public:
        const std::optional<raw::input_event> readNextRawNonBlocking() override;

        void handleGlfwKeyInput(GLFWwindow* window,
                                int key,
                                int scancode,
                                int action,
                                int mods) override
        {
            m_eventQueue.push(glfw_key_input{.key = key, .action = action});
        }

        void handleGlfwMouseInput(GLFWwindow* window,
                                  int button,
                                  int action,
                                  int mods) override
        {
            m_eventQueue.push(glfw_mouse_input{.button = button, .action = action});
        }

      private:
        boost::lockfree::spsc_queue<std::variant<glfw_key_input, glfw_mouse_input>,
                                    boost::lockfree::capacity<1024>>
            m_eventQueue;
    };
}
