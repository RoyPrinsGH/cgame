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
        const std::string key = path.flattened();

        if (const auto found = m_loaded.find(key); found != m_loaded.end())
            return found->second.model;

        const model_data model = loadGltf(m_pak->data(path));

        entry loaded;
        loaded.textures.reserve(model.images.size());

        for (const image_data& image : model.images)
            loaded.textures.push_back(m_backend->uploadTexture(image));

        loaded.model = m_backend->uploadMesh(model.primitives, loaded.textures);

        m_loaded.emplace(key, std::move(loaded));

        return m_loaded.find(key)->second.model;
    }

    void model_controller::unload(const assets::virtual_asset_path& path)
    {
        const std::string key = path.flattened();

        const auto found = m_loaded.find(key);

        if (found == m_loaded.end())
            return;

        for (const texture_handle texture : found->second.textures)
            m_backend->unloadTexture(texture);

        m_backend->unloadModel(found->second.model);

        m_loaded.erase(found);
    }

    void model_controller::unloadAll()
    {
        for (const auto& [key, loaded] : m_loaded)
        {
            for (const texture_handle texture : loaded.textures)
                m_backend->unloadTexture(texture);

            m_backend->unloadModel(loaded.model);
        }

        m_loaded.clear();
    }
}
