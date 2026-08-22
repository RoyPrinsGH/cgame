#pragma once
#include <optional>
#include "raw_input_event.hpp"

namespace engine::input
{
    class raw_input_stream
    {
    public:
        virtual const std::optional<raw::input_event> readNextRawNonBlocking() { return std::nullopt; };
    };
}