#pragma once

#include <cstdint>

namespace cgame::graphics
{
    struct texture_handle
    {
        std::uint32_t id = 0;

        [[nodiscard]]
        bool valid() const
        {
            return id != 0;
        }
    };
}
