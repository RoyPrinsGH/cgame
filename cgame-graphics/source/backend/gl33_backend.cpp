#include <cstddef>
#include <optional>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#define GRAPHICS_API_OPENGL_33
#include <rlgl.h>

#include <external/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>

#include <cgame/graphics/render_backend.hpp>

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
            unsigned int albedoTextureId = 0;
            int indexCount = 0;
            unsigned int drawMode = GL_TRIANGLES;
        };

        struct gpu_model
        {
            std::vector<gpu_primitive> primitives;
            unsigned int instanceVboId = 0;
            int instanceCapacity = 0;
        };

        struct gpu_shader
        {
            unsigned int programId = 0;
            std::optional<int> viewLocation;
            std::optional<int> projectionLocation;
            std::optional<int> albedoLocation;
        };

        std::optional<int> uniformLocation(unsigned int programId, const char* name)
        {
            const int location = rlGetLocationUniform(programId, name);

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
            }

            ~gl33_backend() override
            {
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
                    throw std::runtime_error("could not compile shader program");

                shader.viewLocation = uniformLocation(shader.programId, "matView");
                shader.projectionLocation =
                    uniformLocation(shader.programId, "matProjection");
                shader.albedoLocation =
                    uniformLocation(shader.programId, "baseColorTexture");

                m_shaders.push_back(shader);

                return {static_cast<std::uint32_t>(m_shaders.size())};
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
                    throw std::runtime_error("could not upload texture");

                return {id};
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

                    rlSetVertexAttribute(0, 3, RL_FLOAT, false, sizeof(vertex), 0);
                    rlEnableVertexAttribute(0);
                    rlSetVertexAttribute(2, 3, RL_FLOAT, false, sizeof(vertex),
                                         3 * sizeof(float));
                    rlEnableVertexAttribute(2);
                    rlSetVertexAttribute(1, 2, RL_FLOAT, false, sizeof(vertex),
                                         6 * sizeof(float));
                    rlEnableVertexAttribute(1);

                    gpuPrimitive.indexVboId = rlLoadVertexBufferElement(
                        primitive.indices.data(),
                        static_cast<int>(primitive.indices.size() *
                                         sizeof(std::uint32_t)),
                        false);

                    rlDisableVertexArray();

                    if (primitive.albedoIndex >= 0 &&
                        static_cast<std::size_t>(primitive.albedoIndex) < textures.size())
                        gpuPrimitive.albedoTextureId =
                            textures[primitive.albedoIndex].id;

                    model.primitives.push_back(gpuPrimitive);
                }

                m_models.push_back(std::move(model));

                return {static_cast<std::uint32_t>(m_models.size())};
            }

            void uploadInstances(model_handle handle,
                                 std::span<const glm::mat4> instances) override
            {
                gpu_model& model = modelAt(handle);

                const int count = static_cast<int>(instances.size());

                if (count > model.instanceCapacity)
                    growInstanceBuffer(model, count);

                if (count > 0)
                    rlUpdateVertexBuffer(model.instanceVboId, instances.data(),
                                         count * static_cast<int>(sizeof(glm::mat4)), 0);
            }

            void beginFrame(const glm::mat4& view,
                            const glm::mat4& projection,
                            int fbWidth,
                            int fbHeight) override
            {
                m_view = toRayMatrix(view);
                m_projection = toRayMatrix(projection);

                rlSetFramebufferWidth(fbWidth);
                rlSetFramebufferHeight(fbHeight);
                rlViewport(0, 0, fbWidth, fbHeight);

                rlClearColor(20, 20, 20, 255);
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
                    if (shader.albedoLocation)
                    {
                        int textureSlot = 0;
                        rlActiveTextureSlot(textureSlot);
                        rlEnableTexture(primitive.albedoTextureId);
                        rlSetUniform(*shader.albedoLocation, &textureSlot,
                                     RL_SHADER_UNIFORM_INT, 1);
                    }

                    rlEnableVertexArray(primitive.vaoId);
                    glDrawElementsInstanced(primitive.drawMode, primitive.indexCount,
                                            GL_UNSIGNED_INT, nullptr, instanceCount);
                    rlDisableVertexArray();

                    if (shader.albedoLocation)
                        rlDisableTexture();
                }
            }

            void endFrame() override
            {
                rlDrawRenderBatchActive();
            }

          private:
            void growInstanceBuffer(gpu_model& model, int count)
            {
                if (model.instanceVboId != 0)
                    rlUnloadVertexBuffer(model.instanceVboId);

                const std::vector<glm::mat4> initial(count, glm::mat4(1.0f));

                model.instanceVboId = rlLoadVertexBuffer(
                    initial.data(),
                    static_cast<int>(initial.size() * sizeof(glm::mat4)), true);
                model.instanceCapacity = count;

                for (const gpu_primitive& primitive : model.primitives)
                {
                    rlEnableVertexArray(primitive.vaoId);
                    rlEnableVertexBuffer(model.instanceVboId);

                    for (int column = 0; column < 4; ++column)
                    {
                        const unsigned int location = 9 + column;

                        rlSetVertexAttribute(location, 4, RL_FLOAT, false,
                                             sizeof(glm::mat4),
                                             column * sizeof(glm::vec4));
                        rlEnableVertexAttribute(location);
                        rlSetVertexAttributeDivisor(location, 1);
                    }

                    rlDisableVertexArray();
                }
            }

            gpu_model& modelAt(model_handle handle)
            {
                if (!handle.valid() || handle.id > m_models.size())
                    throw std::runtime_error("unknown model handle");

                return m_models[handle.id - 1];
            }

            const gpu_model& modelAt(model_handle handle) const
            {
                if (!handle.valid() || handle.id > m_models.size())
                    throw std::runtime_error("unknown model handle");

                return m_models[handle.id - 1];
            }

            const gpu_shader& shaderAt(shader_handle handle) const
            {
                if (!handle.valid() || handle.id > m_shaders.size())
                    throw std::runtime_error("unknown shader handle");

                return m_shaders[handle.id - 1];
            }

            std::vector<gpu_model> m_models;
            std::vector<gpu_shader> m_shaders;

            shader_handle m_activeShader;

            Matrix m_view = toRayMatrix(glm::mat4(1.0f));
            Matrix m_projection = toRayMatrix(glm::mat4(1.0f));
        };
    }

    std::unique_ptr<render_backend> createGl33Backend(gl_proc_loader getProcAddress)
    {
        return std::make_unique<gl33_backend>(getProcAddress);
    }
}
