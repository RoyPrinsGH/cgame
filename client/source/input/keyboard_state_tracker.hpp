#pragma once
#include <array>
#include <cstdint>
#include <chrono>
#include <utility>
#include <limits>

namespace engine::input
{
    // Only used from one thread, so no mutex/thread sync magic
    template <typename TClock = std::chrono::steady_clock, size_t Size = UINT8_MAX + 1>
        requires std::chrono::is_clock_v<TClock>
    class keyboard_state_tracker
    {
    public:
        void setState(const uint8_t key, const bool isPressed)
        {
            auto existing = m_keys.at(key);
            if (existing.first == isPressed)
                return;
            m_keys[key] = std::pair(isPressed, TClock::now());
        }
        const std::pair<bool, std::chrono::time_point<TClock>> &getState(const uint8_t key)
        {
            return m_keys[key];
        }

    private:
        std::array<std::pair<bool, std::chrono::time_point<TClock>>, Size> m_keys;
    };
}