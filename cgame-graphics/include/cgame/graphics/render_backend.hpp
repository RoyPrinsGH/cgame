#pragma once

#include <memory>
#include <span>
#include <string_view>

#include <glm/mat4x4.hpp>

#include <cgame/graphics/mesh.hpp>
#include <cgame/graphics/model.hpp>
#include <cgame/graphics/shader.hpp>

namespace cgame::graphics
{
    using gl_proc_loader = void* (*)(const char* procName);

    class render_backend
    {
      public:
        virtual ~render_backend() = default;

        virtual shader_handle loadShader(std::string_view vertexSource,
                                         std::string_view fragmentSource) = 0;

        virtual void activateShader(shader_handle shader) = 0;
        virtual void deactivateShader() = 0;

        virtual model_handle uploadMesh(std::span<const mesh_data> primitives) = 0;

        virtual void uploadInstances(model_handle model,
                                     std::span<const glm::mat4> instances) = 0;

        virtual void beginFrame(const glm::mat4& view,
                                const glm::mat4& projection,
                                int fbWidth,
                                int fbHeight) = 0;

        virtual void draw(model_handle model, int instanceCount) = 0;

        virtual void endFrame() = 0;
    };

    std::unique_ptr<render_backend> createGl33Backend(gl_proc_loader getProcAddress);
}
