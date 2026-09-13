#include <cstddef>
#include <optional>
#include <cstring>
#include <string>
#include <vector>

#define GRAPHICS_API_OPENGL_33
#include <rlgl.h>

#include <external/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>

#include <cgame/graphics/errors.hpp>
#include <cgame/graphics/render_backend.hpp>
#include <cgame/graphics/shader_contract.hpp>

namespace cgame::graphics
{
    namespace
    {
        struct vertex
        {
            float px, py, pz;
            float nx, ny, nz;
            float u, v;
        };

        struct gpu_primitive
        {
            unsigned int vaoId = 0;
            unsigned int vertexVboId = 0;
            unsigned int indexVboId = 0;
            texture_handle albedoTexture;
            int indexCount = 0;
            unsigned int drawMode = GL_TRIANGLES;
        };

        struct gpu_model
        {
            std::vector<gpu_primitive> primitives;
        };

        // Dynamic per-model state: the instance matrix buffer is created and
        // grown by instance uploads, so unlike the immutable resource slots
        // this is mutable runtime state, not a loaded resource.
        struct gpu_instance_stream
        {
            unsigned int vboId = 0;
            int capacity = 0;
        };

        struct gpu_texture
        {
            unsigned int textureId = 0;
        };

        struct gpu_shader
        {
            unsigned int programId = 0;
            std::optional<int> viewLocation;
            std::optional<int> projectionLocation;
            std::optional<int> albedoLocation;
        };

        std::optional<int> uniformLocation(unsigned int programId,
                                           std::string_view name)
        {
            const int location =
                rlGetLocationUniform(programId, std::string(name).c_str());

            if (location < 0)
                return std::nullopt;

            return location;
        }

        Matrix toRayMatrix(const glm::mat4& matrix)
        {
            static_assert(sizeof(Matrix) == sizeof(glm::mat4));

            const glm::mat4 transposed = glm::transpose(matrix);

            Matrix result;
            std::memcpy(&result, glm::value_ptr(transposed), sizeof(Matrix));

            return result;
        }

        class gl33_backend final : public render_backend
        {
          public:
            explicit gl33_backend(gl_proc_loader getProcAddress)
            {
                rlLoadExtensions(reinterpret_cast<void*>(getProcAddress));
                rlglInit(0, 0);
                rlEnableBackfaceCulling();

                // Bound in place of a texture whose handle went stale at
                // draw time, so a dangling reference in a loaded model
                // degrades visibly instead of killing the render.
                const std::uint8_t placeholderPixel[4] = {255, 0, 255, 255};
                m_placeholderTextureId = rlLoadTexture(
                    placeholderPixel, 1, 1, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
            }

            ~gl33_backend() override
            {
                if (m_placeholderTextureId != 0)
                    rlUnloadTexture(m_placeholderTextureId);

                rlglClose();
            }

            shader_handle loadShader(std::string_view vertexSource,
                                     std::string_view fragmentSource) override
            {
                const std::string vertex(vertexSource);
                const std::string fragment(fragmentSource);

                gpu_shader shader;
                shader.programId =
                    rlLoadShaderProgram(vertex.c_str(), fragment.c_str());

                if (shader.programId == 0)
                    throw shader_compile_error("could not compile shader program");

                shader.viewLocation =
                    uniformLocation(shader.programId, shader_contract::viewUniform);
                shader.projectionLocation = uniformLocation(
                    shader.programId, shader_contract::projectionUniform);
                shader.albedoLocation =
                    uniformLocation(shader.programId, shader_contract::albedoSampler);

                if (!shader.viewLocation || !shader.projectionLocation)
                    throw shader_compile_error(
                        "shader missing required uniforms matView/matProjection");

                return allocateShader(std::move(shader));
            }

            void unloadShader(shader_handle handle) override
            {
                const gpu_shader& shader = shaderAt(handle);

                if (m_activeShader == handle)
                {
                    rlDisableShader();
                    m_activeShader = {};
                }

                rlUnloadShaderProgram(shader.programId);

                freeShader(handle);
            }

            void activateShader(shader_handle handle) override
            {
                const gpu_shader& shader = shaderAt(handle);

                rlEnableShader(shader.programId);

                if (shader.viewLocation)
                    rlSetUniformMatrix(*shader.viewLocation, m_view);

                if (shader.projectionLocation)
                    rlSetUniformMatrix(*shader.projectionLocation, m_projection);

                m_activeShader = handle;
            }

            void deactivateShader() override
            {
                rlDisableShader();
                m_activeShader = {};
            }

            texture_handle uploadTexture(const image_data& image) override
            {
                const unsigned int id =
                    rlLoadTexture(image.pixels.data(), image.width, image.height,
                                  RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);

                if (id == 0)
                    throw texture_upload_error("could not upload texture");

                return allocateTexture(id);
            }

            void unloadTexture(texture_handle handle) override
            {
                const gpu_texture& texture = textureAt(handle);

                rlUnloadTexture(texture.textureId);

                freeTexture(handle);
            }

            model_handle uploadMesh(std::span<const primitive_data> primitives,
                                    std::span<const texture_handle> textures) override
            {
                gpu_model model;
                model.primitives.reserve(primitives.size());

                for (const primitive_data& primitive : primitives)
                {
                    std::vector<vertex> vertices;
                    vertices.reserve(primitive.positions.size());

                    for (std::size_t index = 0; index < primitive.positions.size();
                         ++index)
                    {
                        const glm::vec3& position = primitive.positions[index];
                        const glm::vec3& normal = primitive.normals[index];
                        const glm::vec2& texcoord = primitive.texcoords[index];

                        vertices.push_back({position.x, position.y, position.z, normal.x,
                                            normal.y, normal.z, texcoord.x, texcoord.y});
                    }

                    gpu_primitive gpuPrimitive;
                    gpuPrimitive.indexCount = static_cast<int>(primitive.indices.size());
                    gpuPrimitive.drawMode = primitive.topology == mesh_topology::lines
                                                ? GL_LINES
                                                : GL_TRIANGLES;

                    gpuPrimitive.vaoId = rlLoadVertexArray();
                    rlEnableVertexArray(gpuPrimitive.vaoId);

                    gpuPrimitive.vertexVboId = rlLoadVertexBuffer(
                        vertices.data(),
                        static_cast<int>(vertices.size() * sizeof(vertex)), false);
                    rlEnableVertexBuffer(gpuPrimitive.vertexVboId);

                    rlSetVertexAttribute(shader_contract::positionLocation, 3,
                                         RL_FLOAT, false, sizeof(vertex), 0);
                    rlEnableVertexAttribute(shader_contract::positionLocation);
                    rlSetVertexAttribute(shader_contract::normalLocation, 3, RL_FLOAT,
                                         false, sizeof(vertex), 3 * sizeof(float));
                    rlEnableVertexAttribute(shader_contract::normalLocation);
                    rlSetVertexAttribute(shader_contract::texcoordLocation, 2,
                                         RL_FLOAT, false, sizeof(vertex),
                                         6 * sizeof(float));
                    rlEnableVertexAttribute(shader_contract::texcoordLocation);

                    gpuPrimitive.indexVboId = rlLoadVertexBufferElement(
                        primitive.indices.data(),
                        static_cast<int>(primitive.indices.size() *
                                         sizeof(std::uint32_t)),
                        false);

                    rlDisableVertexArray();

                    if (primitive.albedoIndex &&
                        *primitive.albedoIndex < textures.size())
                        gpuPrimitive.albedoTexture =
                            textures[*primitive.albedoIndex];

                    model.primitives.push_back(gpuPrimitive);
                }

                return allocateModel(std::move(model));
            }

            void unloadModel(model_handle handle) override
            {
                const gpu_model& model = modelAt(handle);

                for (const gpu_primitive& primitive : model.primitives)
                {
                    if (primitive.indexVboId != 0)
                        rlUnloadVertexBuffer(primitive.indexVboId);

                    if (primitive.vertexVboId != 0)
                        rlUnloadVertexBuffer(primitive.vertexVboId);

                    if (primitive.vaoId != 0)
                        rlUnloadVertexArray(primitive.vaoId);
                }

                const gpu_instance_stream& stream = instanceStreamAt(handle);

                if (stream.vboId != 0)
                    rlUnloadVertexBuffer(stream.vboId);

                freeModel(handle);
            }

            void uploadInstances(model_handle handle,
                                 std::span<const glm::mat4> instances) override
            {
                const gpu_model& model = modelAt(handle);
                gpu_instance_stream& stream = instanceStreamAt(handle);

                const int count = static_cast<int>(instances.size());

                if (count > stream.capacity)
                    growInstanceStream(model, stream, count);

                if (count > 0)
                    rlUpdateVertexBuffer(stream.vboId, instances.data(),
                                         count * static_cast<int>(sizeof(glm::mat4)), 0);
            }

            void beginFrame(const glm::mat4& view,
                            const glm::mat4& projection,
                            int fbWidth,
                            int fbHeight,
                            const rgba8& clearColour) override
            {
                m_view = toRayMatrix(view);
                m_projection = toRayMatrix(projection);

                rlSetFramebufferWidth(fbWidth);
                rlSetFramebufferHeight(fbHeight);
                rlViewport(0, 0, fbWidth, fbHeight);

                rlClearColor(clearColour.r, clearColour.g, clearColour.b, clearColour.a);
                rlClearScreenBuffers();

                rlSetMatrixProjection(m_projection);
                rlSetMatrixModelview(m_view);
                rlEnableDepthTest();
            }

            void draw(model_handle handle, int instanceCount) override
            {
                if (instanceCount <= 0)
                    return;

                const gpu_model& model = modelAt(handle);
                const gpu_shader& shader = shaderAt(m_activeShader);

                for (const gpu_primitive& primitive : model.primitives)
                {
                    // An albedo handle that went stale (texture unloaded,
                    // slot reused) degrades to the magenta placeholder
                    // instead of throwing - a dangling reference inside a
                    // loaded model is bad data, not API misuse. Everything
                    // else here still throws via the lookups above.
                    bool albedoBound = false;

                    if (shader.albedoLocation)
                    {
                        int textureSlot = 0;
                        rlActiveTextureSlot(textureSlot);

                        if (primitive.albedoTexture.valid())
                        {
                            unsigned int textureId = m_placeholderTextureId;

                            if (textureAlive(primitive.albedoTexture))
                                textureId =
                                    textureAt(primitive.albedoTexture).textureId;

                            rlEnableTexture(textureId);
                            rlSetUniform(*shader.albedoLocation, &textureSlot,
                                         RL_SHADER_UNIFORM_INT, 1);

                            albedoBound = true;
                        }
                    }

                    rlEnableVertexArray(primitive.vaoId);
                    glDrawElementsInstanced(primitive.drawMode, primitive.indexCount,
                                            GL_UNSIGNED_INT, nullptr, instanceCount);
                    rlDisableVertexArray();

                    if (albedoBound)
                        rlDisableTexture();
                }
            }

            void endFrame() override
            {
                rlDrawRenderBatchActive();
            }

          private:
            void growInstanceStream(const gpu_model& model,
                                    gpu_instance_stream& stream,
                                    int count)
            {
                if (stream.vboId != 0)
                    rlUnloadVertexBuffer(stream.vboId);

                const std::vector<glm::mat4> initial(count, glm::mat4(1.0f));

                stream.vboId = rlLoadVertexBuffer(
                    initial.data(),
                    static_cast<int>(initial.size() * sizeof(glm::mat4)), true);
                stream.capacity = count;

                for (const gpu_primitive& primitive : model.primitives)
                {
                    rlEnableVertexArray(primitive.vaoId);
                    rlEnableVertexBuffer(stream.vboId);

                    for (int column = 0; column < 4; ++column)
                    {
                        const unsigned int location =
                            shader_contract::instanceTransformLocation + column;

                        rlSetVertexAttribute(location, 4, RL_FLOAT, false,
                                             sizeof(glm::mat4),
                                             column * sizeof(glm::vec4));
                        rlEnableVertexAttribute(location);
                        rlSetVertexAttributeDivisor(location, 1);
                    }

                    rlDisableVertexArray();
                }
            }

            // Slot primitives with free-list reuse. `generation` is bumped when a
            // slot is released, so a stale handle (old generation) can never
            // resolve to the resource that later reuses the slot.
            static std::uint32_t takeSlot(std::vector<std::uint32_t>& generations,
                                          std::vector<std::uint32_t>& freeIds)
            {
                if (!freeIds.empty())
                {
                    const std::uint32_t id = freeIds.back();
                    freeIds.pop_back();
                    return id;
                }

                generations.push_back(1);
                return static_cast<std::uint32_t>(generations.size());
            }

            static void releaseSlot(std::uint32_t id,
                                    std::vector<std::uint32_t>& generations,
                                    std::vector<std::uint32_t>& freeIds)
            {
                ++generations[id - 1];
                freeIds.push_back(id);
            }

            shader_handle allocateShader(gpu_shader shader)
            {
                const std::uint32_t slot = takeSlot(m_shaderGenerations, m_freeShaderIds);
                const shader_handle handle{slot, m_shaderGenerations[slot - 1]};

                m_shaders.resize(slot);
                m_shaders[slot - 1] = std::move(shader);

                return handle;
            }

            void freeShader(shader_handle handle)
            {
                m_shaders[handle.id - 1] = {};
                releaseSlot(handle.id, m_shaderGenerations, m_freeShaderIds);
            }

            texture_handle allocateTexture(unsigned int textureId)
            {
                const std::uint32_t slot =
                    takeSlot(m_textureGenerations, m_freeTextureIds);
                const texture_handle handle{slot, m_textureGenerations[slot - 1]};

                m_textures.resize(slot);
                m_textures[slot - 1] = {textureId};

                return handle;
            }

            void freeTexture(texture_handle handle)
            {
                m_textures[handle.id - 1] = {};
                releaseSlot(handle.id, m_textureGenerations, m_freeTextureIds);
            }

            model_handle allocateModel(gpu_model model)
            {
                const std::uint32_t slot = takeSlot(m_modelGenerations, m_freeModelIds);
                const model_handle handle{slot, m_modelGenerations[slot - 1]};

                m_models.resize(slot);
                m_models[slot - 1] = std::move(model);

                // Instance streams live and die with their model slot.
                m_instanceStreams.resize(slot);
                m_instanceStreams[slot - 1] = {};

                return handle;
            }

            void freeModel(model_handle handle)
            {
                m_models[handle.id - 1] = {};
                m_instanceStreams[handle.id - 1] = {};
                releaseSlot(handle.id, m_modelGenerations, m_freeModelIds);
            }

            // Shared lookup for all three resource tables. A handle is only
            // usable while its generation matches the live slot generation;
            // anything else throws instead of silently aliasing the resource
            // that now owns the slot.
            template <typename Resource>
            static const Resource& lookup(const std::vector<Resource>& slots,
                                          const std::vector<std::uint32_t>& generations,
                                          std::uint32_t id,
                                          std::uint32_t generation,
                                          const char* error)
            {
                if (id == 0 || id > slots.size() || generations[id - 1] != generation)
                    throw bad_handle_error(error);

                return slots[id - 1];
            }

            const gpu_model& modelAt(model_handle handle) const
            {
                return lookup(m_models, m_modelGenerations, handle.id,
                              handle.generation, "unknown model handle");
            }

            const gpu_shader& shaderAt(shader_handle handle) const
            {
                return lookup(m_shaders, m_shaderGenerations, handle.id,
                              handle.generation, "unknown shader handle");
            }

            const gpu_texture& textureAt(texture_handle handle) const
            {
                return lookup(m_textures, m_textureGenerations, handle.id,
                              handle.generation, "unknown texture handle");
            }

            // Non-throwing liveness check, used at draw time to degrade a
            // stale albedo reference instead of throwing from inside a frame.
            bool textureAlive(texture_handle handle) const
            {
                return handle.valid() && handle.id <= m_textures.size() &&
                       m_textureGenerations[handle.id - 1] == handle.generation;
            }

            // Instance streams are written to by instance uploads, so unlike
            // the immutable resource tables above this only has a mutable
            // accessor; there is nothing const to read here.
            gpu_instance_stream& instanceStreamAt(model_handle handle)
            {
                if (!handle.valid() || handle.id > m_instanceStreams.size() ||
                    handle.generation != m_modelGenerations[handle.id - 1])
                    throw bad_handle_error("unknown model handle");

                return m_instanceStreams[handle.id - 1];
            }

            std::vector<gpu_model> m_models;
            std::vector<std::uint32_t> m_modelGenerations;
            std::vector<std::uint32_t> m_freeModelIds;

            // Parallel to m_models: same slot ids, same generations/free list.
            std::vector<gpu_instance_stream> m_instanceStreams;

            std::vector<gpu_texture> m_textures;
            std::vector<std::uint32_t> m_textureGenerations;
            std::vector<std::uint32_t> m_freeTextureIds;

            std::vector<gpu_shader> m_shaders;
            std::vector<std::uint32_t> m_shaderGenerations;
            std::vector<std::uint32_t> m_freeShaderIds;

            shader_handle m_activeShader;

            // 1×1 magenta, bound for stale albedo handles at draw time.
            unsigned int m_placeholderTextureId = 0;

            Matrix m_view = toRayMatrix(glm::mat4(1.0f));
            Matrix m_projection = toRayMatrix(glm::mat4(1.0f));
        };
    }

    std::unique_ptr<render_backend> createGl33Backend(gl_proc_loader getProcAddress)
    {
        return std::make_unique<gl33_backend>(getProcAddress);
    }
}
