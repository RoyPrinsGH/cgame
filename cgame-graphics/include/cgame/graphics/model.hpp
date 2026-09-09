#pragma once

#include <cstdint>

namespace cgame::graphics
{
    // Handles stay valid across backend slot reuse: `generation` is bumped every
    // time the backend frees a slot, so a stale handle is detected instead of
    // silently addressing a different resource.
    struct model_handle
    {
        std::uint32_t id = 0;
        std::uint32_t generation = 0;

        [[nodiscard]]
        bool valid() const
        {
            return id != 0;
        }

        [[nodiscard]]
        bool operator==(const model_handle& other) const = default;
    };
}
