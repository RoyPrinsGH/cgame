#pragma once

#include <cstdint>

namespace cgame::graphics
{
    struct shader_handle
    {
        std::uint32_t id = 0;

        [[nodiscard]]
        bool valid() const
        {
            return id != 0;
        }
    };
}
