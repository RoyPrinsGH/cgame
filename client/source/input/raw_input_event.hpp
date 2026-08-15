#pragma once
#include <variant>

namespace engine::input::raw
{
    enum special_key
    {
        escape,
        enter
    };

    struct special_key_up
    {
        special_key key;
    };

    struct special_key_down
    {
        special_key key;
    };

    struct char_key_down
    {
        char key;
    };

    struct char_key_up
    {
        char key;
    };

    struct mouse_move
    {
        float dx;
        float dy;
    };

    enum mouse_button
    {
        left,
        middle,
        right
    };

    struct mouse_button_down
    {
        mouse_button button;
    };

    struct mouse_button_up
    {
        mouse_button button;
    };

    struct other
    {
    };

    using input_event = std::variant<
        special_key_up,
        special_key_down,
        char_key_down,
        char_key_up,
        mouse_move,
        mouse_button_up,
        mouse_button_down,
        other>;
}