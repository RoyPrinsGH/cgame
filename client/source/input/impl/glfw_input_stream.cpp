#include "glfw_input_stream.hpp"

namespace engine::input::glfw
{
    const std::optional<raw::input_event> glfw_raw_input_stream::readNextRawNonBlocking()
    {
        std::variant<glfw_key_input, glfw_mouse_input> glfwInput;
        if (!m_eventQueue.pop(glfwInput))
            return std::nullopt;

        if (auto *keyInput = std::get_if<glfw_key_input>(&glfwInput))
        {
            if (GLFW_KEY_A <= keyInput->key && keyInput->key <= GLFW_KEY_Z)
            {
                switch (keyInput->action)
                {
                case GLFW_PRESS:
                    return raw::char_key_down{.key = (uint8_t)keyInput->key};
                case GLFW_RELEASE:
                    return raw::char_key_up{.key = (uint8_t)keyInput->key};
                default:
                    return raw::other{};
                };
            }

            return raw::other{};
        }
        else if (auto *mouseInput = std::get_if<glfw_mouse_input>(&glfwInput))
        {
            raw::mouse_button mouseButton;

            switch (mouseInput->button)
            {
            case GLFW_MOUSE_BUTTON_LEFT:
                mouseButton = raw::mouse_button::left;
                break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
                mouseButton = raw::mouse_button::middle;
                break;
            case GLFW_MOUSE_BUTTON_RIGHT:
                mouseButton = raw::mouse_button::right;
                break;
            default:
                return raw::other{};
            }

            switch (mouseInput->action)
            {
            case GLFW_PRESS:
                return raw::mouse_button_down{.button = mouseButton};
            case GLFW_RELEASE:
                return raw::mouse_button_up{.button = mouseButton};
            default:
                return raw::other{};
            };
        }
    }
}
