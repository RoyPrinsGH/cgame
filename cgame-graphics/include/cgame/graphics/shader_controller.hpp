#pragma once

#include <string>
#include <unordered_map>

#include <cgame/assets/pak.hpp>

#include <cgame/graphics/render_backend.hpp>
#include <cgame/graphics/shader.hpp>

namespace cgame::graphics
{
    class shader_controller
    {
      public:
        shader_controller(render_backend* backend, assets::pak* pak);

        shader_handle load(const assets::virtual_asset_path& vertexPath,
                           const assets::virtual_asset_path& fragmentPath);

        // Evicts the compiled program. Using a handle returned by a former
        // load() afterwards throws in the backend (generation counter), it does
        // not alias a newer resource.
        void unload(const assets::virtual_asset_path& vertexPath,
                    const assets::virtual_asset_path& fragmentPath);

        void unloadAll();

      private:
        static std::string keyOf(const assets::virtual_asset_path& vertexPath,
                                 const assets::virtual_asset_path& fragmentPath);

        render_backend* m_backend = nullptr;
        assets::pak* m_pak = nullptr;

        std::unordered_map<std::string, shader_handle> m_loaded;
    };
}
