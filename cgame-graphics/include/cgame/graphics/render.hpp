#pragma once

#include <cgame/graphics/render_backend.hpp>
#include <cgame/graphics/render_snapshot.hpp>

namespace cgame::graphics
{
    void render(render_backend& backend,
                const render_snapshot& snapshot,
                int fbWidth,
                int fbHeight);
}
