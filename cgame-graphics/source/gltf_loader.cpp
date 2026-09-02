#include <cstdint>
#include <stdexcept>
#include <utility>
#include <variant>

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>

#include <stb_image.h>

#include "gltf_loader.hpp"

namespace cgame::graphics
{
    namespace
    {
        image_data decodeImage(const fastgltf::Asset& asset, const fastgltf::Image& image)
        {
            int width = 0;
            int height = 0;
            int channels = 0;

            unsigned char* pixels = nullptr;

            std::visit(
                fastgltf::visitor{
                    [](auto&) {},

                    [&](const fastgltf::sources::Array& source)
                    {
                        pixels = stbi_load_from_memory(
                            reinterpret_cast<const stbi_uc*>(source.bytes.data()),
                            static_cast<int>(source.bytes.size()), &width, &height,
                            &channels, 4);
                    },

                    [&](const fastgltf::sources::BufferView& source)
                    {
                        const auto& view = asset.bufferViews[source.bufferViewIndex];

                        const auto& buffer = asset.buffers[view.bufferIndex];

                        std::visit(
                            fastgltf::visitor{[](auto&) {},

                                              [&](const fastgltf::sources::Array& bytes)
                                              {
                                                  pixels = stbi_load_from_memory(
                                                      reinterpret_cast<const stbi_uc*>(
                                                          bytes.bytes.data() +
                                                          view.byteOffset),
                                                      static_cast<int>(view.byteLength),
                                                      &width, &height, &channels, 4);
                                              }},
                            buffer.data);
                    }},
                image.data);

            if (!pixels)
                throw std::runtime_error(stbi_failure_reason());

            image_data decoded;
            decoded.width = width;
            decoded.height = height;
            decoded.channels = 4;
            decoded.pixels.assign(pixels, pixels + static_cast<std::size_t>(width) *
                                                       height * 4);

            stbi_image_free(pixels);

            return decoded;
        }
    }

    model_data loadGltf(std::span<const std::byte> bytes)
    {
        fastgltf::Parser parser;

        auto file = fastgltf::GltfDataBuffer::FromBytes(bytes.data(), bytes.size());

        if (!file)
            throw std::runtime_error("could not open glb");

        auto loaded =
            parser.loadGltf(file.get(), {}, fastgltf::Options::GenerateMeshIndices);

        if (loaded.error() != fastgltf::Error::None)
            throw std::runtime_error("could not parse glb");

        fastgltf::Asset asset = std::move(loaded.get());

        model_data model;
        model.images.reserve(asset.images.size());

        for (const auto& image : asset.images)
        {
            model.images.push_back(decodeImage(asset, image));
        }

        if (asset.meshes.empty())
            throw std::runtime_error("glb contains no meshes");

        for (const auto& mesh : asset.meshes)
        {
            for (const auto& primitive : mesh.primitives)
            {
                auto pos_it = primitive.findAttribute("POSITION");
                if (pos_it == primitive.attributes.end())
                    throw std::runtime_error("mesh has no POSITION");

                const auto& pos_accessor = asset.accessors[pos_it->accessorIndex];
                std::vector<fastgltf::math::fvec3> positions(pos_accessor.count);
                fastgltf::copyFromAccessor<fastgltf::math::fvec3>(asset, pos_accessor,
                                                                  positions.data());

                std::vector<fastgltf::math::fvec3> normals(
                    positions.size(), fastgltf::math::fvec3(0.0f, 1.0f, 0.0f));
                auto normal_it = primitive.findAttribute("NORMAL");
                if (normal_it != primitive.attributes.end())
                    fastgltf::copyFromAccessor<fastgltf::math::fvec3>(
                        asset, asset.accessors[normal_it->accessorIndex], normals.data());

                const auto& index_accessor =
                    asset.accessors[primitive.indicesAccessor.value()];
                std::vector<std::uint32_t> indices(index_accessor.count);
                fastgltf::copyFromAccessor<std::uint32_t>(asset, index_accessor,
                                                          indices.data());

                std::vector<fastgltf::math::fvec2> uvs(positions.size(),
                                                       fastgltf::math::fvec2(0.0f, 0.0f));

                auto uv_it = primitive.findAttribute("TEXCOORD_0");
                if (uv_it != primitive.attributes.end())
                    fastgltf::copyFromAccessor<fastgltf::math::fvec2>(
                        asset, asset.accessors[uv_it->accessorIndex], uvs.data());

                primitive_data data;

                data.positions.reserve(positions.size());
                data.normals.reserve(positions.size());
                data.texcoords.reserve(positions.size());

                for (std::size_t vertex = 0; vertex < positions.size(); ++vertex)
                {
                    const auto& p = positions[vertex];
                    const auto& n = normals[vertex];
                    const auto& uv = uvs[vertex];

                    data.positions.push_back({p.x(), p.y(), p.z()});
                    data.normals.push_back({n.x(), n.y(), n.z()});
                    data.texcoords.push_back({uv.x(), uv.y()});
                }

                data.indices = std::move(indices);

                if (primitive.materialIndex)
                {
                    const auto& material = asset.materials[*primitive.materialIndex];
                    if (material.pbrData.baseColorTexture)
                    {
                        const auto textureIndex =
                            material.pbrData.baseColorTexture->textureIndex;
                        const auto& gltfTexture = asset.textures[textureIndex];

                        if (gltfTexture.imageIndex)
                            data.albedoIndex = static_cast<int>(*gltfTexture.imageIndex);
                    }
                }

                model.primitives.push_back(std::move(data));
            }
        }

        return model;
    }
}
