#pragma once
#include "glfw_input_stream.hpp"

namespace engine::input::glfw
{
    const std::optional<raw::input_event> glfw_raw_input_stream::readNextRawNonBlocking()
    {
        auto maybeGlfwKeyPress = m_eventQueue.pop();
        if (!maybeGlfwKeyPress.has_value())
            return std::nullopt;

        auto glfwKeyPress = maybeGlfwKeyPress.value();

        if (GLFW_KEY_A <= glfwKeyPress.key <= GLFW_KEY_Z)
        {
            switch (glfwKeyPress.action)
            {
            case GLFW_PRESS:
                return raw::char_key_down{.key = (char)glfwKeyPress.key};
            case GLFW_RELEASE:
                return raw::char_key_up{.key = (char)glfwKeyPress.key};
            default:
                return raw::other{};
            };
        }

        return raw::other{};
    }
}
