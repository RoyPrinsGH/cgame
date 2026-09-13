#pragma once

#include <cstdint>

namespace cgame::graphics
{
    // Byte RGBA, matching how the backends consume clear colours directly.
    struct rgba8
    {
        std::uint8_t r = 0;
        std::uint8_t g = 0;
        std::uint8_t b = 0;
        std::uint8_t a = 255;
    };
}
