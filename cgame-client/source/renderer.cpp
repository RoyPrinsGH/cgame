#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/filesystem/path.hpp>

#define RAYMATH_STATIC_INLINE
#include <raymath.h>

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

#include <rlgl.h>

#include <iostream>

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <glm/gtc/quaternion.hpp>

#include "renderer.hpp"

// #define RAYGUI_IMPLEMENTATION
// #include <raygui.h>

namespace engine
{
    struct vertex
    {
        float px, py, pz;
        float nx, ny, nz;
        float u, v;
    };

    struct model_instance
    {
        glm::vec3 position;
        glm::quat rotation;
    };

    static std::string readTextFile(const std::filesystem::path &path)
    {
        std::ifstream file(path);

        if (!file)
            throw std::runtime_error("could not open " + path.string());

        return std::string(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>());
    }

    static Vector3 glmVecToRayMathVec(glm::vec3 glmVec)
    {
        return Vector3{glmVec.x, glmVec.y, glmVec.z};
    };

    gpu_model loadModel(const std::filesystem::path &path, int max_instances)
    {
        fastgltf::Parser parser;

        auto file = fastgltf::MappedGltfFile::FromPath(path);

        if (!file)
            throw std::runtime_error("could not open glb");

        auto loaded = parser.loadGltf(file.get(), path.parent_path(), fastgltf::Options::LoadExternalBuffers | fastgltf::Options::GenerateMeshIndices);

        if (loaded.error() != fastgltf::Error::None)
            throw std::runtime_error("could not parse glb");

        fastgltf::Asset asset = std::move(loaded.get());

        if (asset.meshes.empty())
            throw std::runtime_error("glb contains no meshes");

        gpu_model model;

        std::vector<glm::mat4> initial(max_instances, glm::mat4(1.0f));
        model.instance_vbo = rlLoadVertexBuffer(initial.data(), static_cast<int>(initial.size() * sizeof(glm::mat4)), true);

        for (const auto &mesh : asset.meshes)
        {
            for (const auto &primitive : mesh.primitives)
            {
                auto pos_it = primitive.findAttribute("POSITION");
                if (pos_it == primitive.attributes.end())
                    throw std::runtime_error("mesh has no POSITION");

                const auto &pos_accessor = asset.accessors[pos_it->accessorIndex];
                std::vector<fastgltf::math::fvec3> positions(pos_accessor.count);
                fastgltf::copyFromAccessor<fastgltf::math::fvec3>(asset, pos_accessor, positions.data());

                std::vector<fastgltf::math::fvec3> normals(positions.size(), fastgltf::math::fvec3(0.0f, 1.0f, 0.0f));
                auto normal_it = primitive.findAttribute("NORMAL");
                if (normal_it != primitive.attributes.end())
                    fastgltf::copyFromAccessor<fastgltf::math::fvec3>(asset, asset.accessors[normal_it->accessorIndex], normals.data());

                const auto &index_accessor = asset.accessors[primitive.indicesAccessor.value()];
                std::vector<std::uint32_t> indices(index_accessor.count);
                fastgltf::copyFromAccessor<std::uint32_t>(asset, index_accessor, indices.data());

                std::vector<fastgltf::math::fvec2> uvs(positions.size(), fastgltf::math::fvec2(0.0f, 0.0f));

                auto uv_it = primitive.findAttribute("TEXCOORD_0");
                if (uv_it != primitive.attributes.end())
                    fastgltf::copyFromAccessor<fastgltf::math::fvec2>(asset, asset.accessors[uv_it->accessorIndex], uvs.data());

                std::vector<vertex> vertices;
                vertices.reserve(indices.size());

                for (const auto index : indices)
                {
                    const auto &p = positions[index];
                    const auto &n = normals[index];
                    const auto &uv = uvs[index];

                    vertices.push_back({p.x(), p.y(), p.z(), n.x(), n.y(), n.z(), uv.x(), uv.y()});
                }

                gpu_primitive gpuPrimitive;

                gpuPrimitive.vertex_count = static_cast<int>(vertices.size());
                gpuPrimitive.vao = rlLoadVertexArray();
                rlEnableVertexArray(gpuPrimitive.vao);
                gpuPrimitive.vertex_vbo = rlLoadVertexBuffer(vertices.data(), static_cast<int>(vertices.size() * sizeof(vertex)), false);
                rlEnableVertexBuffer(gpuPrimitive.vertex_vbo);
                rlSetVertexAttribute(0, 3, RL_FLOAT, false, sizeof(vertex), 0); // position
                rlEnableVertexAttribute(0);
                rlSetVertexAttribute(2, 3, RL_FLOAT, false, sizeof(vertex), 3 * sizeof(float)); // normal
                rlEnableVertexAttribute(2);
                rlSetVertexAttribute(1, 2, RL_FLOAT, false, sizeof(vertex), 6 * sizeof(float)); // texture coords
                rlEnableVertexAttribute(1);

                rlEnableVertexBuffer(model.instance_vbo);
                for (int column = 0; column < 4; column++)
                {
                    const unsigned int location = 9 + column;
                    rlSetVertexAttribute(location, 4, RL_FLOAT, false, sizeof(glm::mat4), column * sizeof(glm::vec4));
                    rlEnableVertexAttribute(location);
                    rlSetVertexAttributeDivisor(location, 1);
                }
                rlDisableVertexArray();

                if (primitive.materialIndex)
                {
                    const auto &material = asset.materials[*primitive.materialIndex];
                    if (material.pbrData.baseColorTexture)
                    {
                        const auto texture_index = material.pbrData.baseColorTexture->textureIndex;
                        const auto &texture = asset.textures[texture_index];
                        const auto image_index = texture.imageIndex.value();
                        const auto &image = asset.images[image_index];
                        // decode image.data -> RGBA bytes
                        // upload with rlLoadTexture(...)
                    }
                }

                // gpuPrimitive.base_color_texture = rlLoadTexture(rgba_pixels, width, height, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);

                model.primitives.push_back(gpuPrimitive);
            }
        }

        std::cout << "vao supported: " << rlGetVersion() << '\n';

        return model;
    }

    renderer::renderer()
    {
        const std::string vertexShaderCode = readTextFile("/home/rvne/development/cgame/cgame-client/shaders/default.vert");
        const std::string fragmentShaderCode = readTextFile("/home/rvne/development/cgame/cgame-client/shaders/default.frag");

        m_defaultShader = rlLoadShaderProgram(vertexShaderCode.c_str(), fragmentShaderCode.c_str());

        std::cout << m_defaultShader;

        m_defaultShaderViewMatrixLocation = rlGetLocationUniform(m_defaultShader, "matView");
        m_defaultShaderProjectionMatrixLocation = rlGetLocationUniform(m_defaultShader, "matProjection");
        m_defaultShaderBaseColorTextureLocation = rlGetLocationUniform(m_defaultShader, "baseColorTexture");

        m_shipModel = loadModel("/home/rvne/development/cgame/cgame-client/assets/ship.glb", 256);
    }

    void drawGrid(int slices, float spacing)
    {
        const int half = slices / 2;

        rlBegin(RL_LINES);

        for (int i = -half; i <= half; ++i)
        {
            rlColor3f(0.5f, 0.5f, 0.5f);

            rlVertex3f(i * spacing, 0.0f, -half * spacing);
            rlVertex3f(i * spacing, 0.0f, half * spacing);

            rlVertex3f(-half * spacing, 0.0f, i * spacing);
            rlVertex3f(half * spacing, 0.0f, i * spacing);
        }

        rlEnd();
    }

    glm::mat4 makeTransform(const model_instance &instance)
    {
        return glm::scale(
            glm::translate(
                glm::mat4(1.0f), instance.position) *
                glm::mat4_cast(instance.rotation),
            glm::vec3(0.1f));
    }

    model_instance getModelInstanceDataFromShip(const world::ship &ship)
    {
        return model_instance{.position = ship.position, .rotation = ship.rotation};
    }

    void renderer::draw(
        GLFWwindow *window,
        const world::game_state &gameState,
        const camera &camera) const
    {
        int width;
        int height;
        glfwGetFramebufferSize(window, &width, &height);

        rlViewport(0, 0, width, height);
        rlClearColor(20, 20, 20, 255);
        rlClearScreenBuffers();

        const double aspectRatio =
            static_cast<double>(width) /
            static_cast<double>(height);

        Matrix projection = MatrixPerspective(
            camera.fov_y * DEG2RAD,
            aspectRatio,
            camera.near_plane,
            camera.far_plane);

        Matrix view = MatrixLookAt(
            glmVecToRayMathVec(camera.position),
            glmVecToRayMathVec(camera.target),
            glmVecToRayMathVec(camera.up));

        rlSetMatrixProjection(projection);
        rlSetMatrixModelview(view);
        rlEnableDepthTest();

        drawGrid(1000, 1.0f);

        rlDrawRenderBatchActive();

        std::vector<glm::mat4> matrices;
        matrices.reserve(gameState.m_enemyShips.size() + 1);

        matrices.push_back(makeTransform(getModelInstanceDataFromShip(gameState.m_playerShip)));

        for (const auto &ship : gameState.m_enemyShips)
            matrices.push_back(makeTransform(getModelInstanceDataFromShip(ship)));

        rlUpdateVertexBuffer(
            m_shipModel.instance_vbo,
            matrices.data(),
            static_cast<int>(
                matrices.size() * sizeof(glm::mat4)),
            0);

        rlEnableShader(m_defaultShader);

        rlSetUniformMatrix(m_defaultShaderViewMatrixLocation, view);
        rlSetUniformMatrix(m_defaultShaderProjectionMatrixLocation, projection);

        for (const auto &primitive : m_shipModel.primitives)
        {
            rlActiveTextureSlot(0);
            rlEnableTexture(primitive.base_color_texture);
            rlSetUniformSampler(m_defaultShaderBaseColorTextureLocation, primitive.base_color_texture);
            rlEnableVertexArray(primitive.vao);
            rlDrawVertexArrayInstanced(0, primitive.vertex_count, static_cast<int>(gameState.m_enemyShips.size()) + 1);
            rlDisableVertexArray();
            rlDisableTexture();
        }

        rlDisableShader();

        rlDrawRenderBatchActive();
        glfwSwapBuffers(window);
    }

    void renderer::drawShip(world::ship ship)
    {
        // Model instance = m_shipModel;
        // auto shipRotation = ship.rotation;
        // Quaternion raylibQuat = {shipRotation.x, shipRotation.y, shipRotation.z, shipRotation.w};
        // instance.transform = QuaternionToMatrix(raylibQuat);
        // auto glmShipPosition = ship.position;
        // DrawModel(instance, {glmShipPosition.x, glmShipPosition.y, glmShipPosition.z}, 0.05f, WHITE);
    }
}