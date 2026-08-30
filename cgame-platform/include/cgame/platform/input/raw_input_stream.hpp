#pragma once

#include <optional>

#include <cgame/platform/input/raw_input_event.hpp>

namespace cgame::platform::input
{
    class raw_input_stream
    {
      public:
        virtual ~raw_input_stream() = default;

        virtual const std::optional<raw::input_event> readNextRawNonBlocking()
        {
            return std::nullopt;
        };
    };
}
