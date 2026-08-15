#pragma once
#include <array>
#include <cstdint>
#include <chrono>
#include <utility>
#include <limits>

namespace engine::input
{
    // Only used from one thread, so no mutex/thread sync magic
    template <typename TClock = std::chrono::steady_clock, size_t KeyboardSize = UINT8_MAX + 1>
        requires std::chrono::is_clock_v<TClock>
    class button_state_tracker
    {
    public:
        void setKeyState(const uint8_t key, const bool isPressed) { this->setButtonState(key + 3, isPressed); }
        const std::pair<bool, std::chrono::time_point<TClock>> &getKeyState(const uint8_t key) const { return m_keys[key + 3]; }
        void setMouseButtonState(const uint8_t button, const bool isPressed) { this->setButtonState(button, isPressed); }
        const std::pair<bool, std::chrono::time_point<TClock>> &getMouseButtonState(const uint8_t button) const { return m_keys[button]; }

    private:
        std::array<std::pair<bool, std::chrono::time_point<TClock>>, KeyboardSize + 3> m_keys;
        void setButtonState(const uint8_t buttonIx, const bool isPressed)
        {
            auto existing = m_keys.at(buttonIx);
            if (existing.first == isPressed)
                return;
            m_keys[buttonIx] = std::pair(isPressed, TClock::now());
        }
    };
}