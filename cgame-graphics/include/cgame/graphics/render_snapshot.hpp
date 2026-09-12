#pragma once

#include <span>
#include <vector>

#include <glm/mat4x4.hpp>

#include <cgame/graphics/camera.hpp>
#include <cgame/graphics/model.hpp>
#include <cgame/graphics/shader.hpp>

namespace cgame::graphics
{
    struct render_entry
    {
        shader_handle shader;
        model_handle model;
        std::span<const glm::mat4> instances;
    };

    struct render_snapshot
    {
        graphics::camera camera;

        // Framebuffer dimensions of the target for this frame; render() uses
        // them for the projection aspect ratio and hands them to the
        // backend's beginFrame() for the viewport.
        int framebufferWidth = 0;
        int framebufferHeight = 0;

        std::vector<render_entry> entries;
    };
}
