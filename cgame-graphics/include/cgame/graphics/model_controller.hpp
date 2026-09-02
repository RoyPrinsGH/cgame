#pragma once

#include <string>
#include <unordered_map>

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

      private:
        static std::string keyOf(const assets::virtual_asset_path& path);

        render_backend* m_backend = nullptr;
        assets::pak* m_pak = nullptr;

        std::unordered_map<std::string, model_handle> m_loaded;
    };
}
