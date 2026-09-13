#pragma once

#include <memory>
#include <span>
#include <string_view>

#include <glm/mat4x4.hpp>

#include <cgame/graphics/colour.hpp>
#include <cgame/graphics/mesh.hpp>
#include <cgame/graphics/model.hpp>
#include <cgame/graphics/shader.hpp>
#include <cgame/graphics/texture.hpp>

namespace cgame::graphics
{
    using gl_proc_loader = void* (*)(const char* procName);

    // Error contract for implementations:
    //
    //   - misuse of a handle (id 0, out of range, stale generation) throws
    //     bad_handle_error — that is a programming error, not bad data
    //   - bad or unsupported data (shader sources, images, meshes) throws a
    //     graphics_error subtype (shader_compile_error, texture_upload_error,
    //     asset_error); these are recoverable, see errors.hpp
    class render_backend
    {
      public:
        virtual ~render_backend() = default;

        virtual shader_handle loadShader(std::string_view vertexSource,
                                         std::string_view fragmentSource) = 0;

        virtual void unloadShader(shader_handle shader) = 0;

        virtual void activateShader(shader_handle shader) = 0;
        virtual void deactivateShader() = 0;

        virtual texture_handle uploadTexture(const image_data& image) = 0;

        virtual void unloadTexture(texture_handle texture) = 0;

        virtual model_handle uploadMesh(std::span<const primitive_data> primitives,
                                        std::span<const texture_handle> textures) = 0;

        virtual void unloadModel(model_handle model) = 0;

        virtual void uploadInstances(model_handle model,
                                     std::span<const glm::mat4> instances) = 0;

        virtual void beginFrame(const glm::mat4& view,
                                const glm::mat4& projection,
                                int fbWidth,
                                int fbHeight,
                                const rgba8& clearColour) = 0;

        virtual void draw(model_handle model, int instanceCount) = 0;

        virtual void endFrame() = 0;
    };

    std::unique_ptr<render_backend> createGl33Backend(gl_proc_loader getProcAddress);
}
