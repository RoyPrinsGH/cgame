#pragma once

#include <cstdint>

namespace cgame::graphics
{
    // See model.hpp for the generation-counter rationale.
    struct shader_handle
    {
        std::uint32_t id = 0;
        std::uint32_t generation = 0;

        [[nodiscard]]
        bool valid() const
        {
            return id != 0;
        }

        [[nodiscard]]
        bool operator==(const shader_handle& other) const = default;
    };
}
