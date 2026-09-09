#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <cgame/assets/pak.hpp>

#include <cgame/graphics/model.hpp>
#include <cgame/graphics/render_backend.hpp>

namespace cgame::graphics
{
    class model_controller
    {
      public:
        model_controller(render_backend* backend, assets::pak* pak);

        model_handle load(const assets::virtual_asset_path& path);

        // Evicts the model and every texture uploaded alongside it. Using a
        // handle returned by a former load() afterwards throws in the backend
        // (generation counter), it does not alias a newer resource.
        void unload(const assets::virtual_asset_path& path);

        void unloadAll();

      private:
        struct entry
        {
            model_handle model;
            std::vector<texture_handle> textures;
        };

        static std::string keyOf(const assets::virtual_asset_path& path);

        render_backend* m_backend = nullptr;
        assets::pak* m_pak = nullptr;

        std::unordered_map<std::string, entry> m_loaded;
    };
}
