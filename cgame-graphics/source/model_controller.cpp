#include <string>
#include <vector>

#include <cgame/graphics/model_controller.hpp>

#include "gltf_loader.hpp"

namespace cgame::graphics
{
    model_controller::model_controller(render_backend* backend, assets::pak* pak)
        : m_backend(backend), m_pak(pak)
    {
    }

    model_handle model_controller::load(const assets::virtual_asset_path& path)
    {
        const std::string key = keyOf(path);

        if (const auto found = m_loaded.find(key); found != m_loaded.end())
            return found->second;

        const model_data model = loadGltf(m_pak->data(path));

        std::vector<texture_handle> textures;
        textures.reserve(model.images.size());

        for (const image_data& image : model.images)
            textures.push_back(m_backend->uploadTexture(image));

        const model_handle handle = m_backend->uploadMesh(model.primitives, textures);

        m_loaded.emplace(key, handle);

        return handle;
    }

    std::string model_controller::keyOf(const assets::virtual_asset_path& path)
    {
        std::string key;

        for (const std::string& part : path.pathParts())
        {
            key += part;
            key += '/';
        }

        return key;
    }
}
