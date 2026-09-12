#include <cstddef>
#include <span>
#include <string>
#include <string_view>

#include <cgame/graphics/shader_controller.hpp>

namespace cgame::graphics
{
    namespace
    {
        std::string_view asText(std::span<const std::byte> bytes)
        {
            return {reinterpret_cast<const char*>(bytes.data()), bytes.size()};
        }
    }

    shader_controller::shader_controller(render_backend* backend, assets::pak* pak)
        : m_backend(backend), m_pak(pak)
    {
    }

    shader_handle shader_controller::load(const assets::virtual_asset_path& vertexPath,
                                          const assets::virtual_asset_path& fragmentPath)
    {
        const std::string key = vertexPath.flattened() + '\n' + fragmentPath.flattened();

        if (const auto found = m_loaded.find(key); found != m_loaded.end())
            return found->second;

        const shader_handle handle = m_backend->loadShader(
            asText(m_pak->data(vertexPath)), asText(m_pak->data(fragmentPath)));

        m_loaded.emplace(key, handle);

        return handle;
    }

    void shader_controller::unload(const assets::virtual_asset_path& vertexPath,
                                   const assets::virtual_asset_path& fragmentPath)
    {
        const std::string key = vertexPath.flattened() + '\n' + fragmentPath.flattened();

        const auto found = m_loaded.find(key);

        if (found == m_loaded.end())
            return;

        m_backend->unloadShader(found->second);

        m_loaded.erase(found);
    }

    void shader_controller::unloadAll()
    {
        for (const auto& [key, handle] : m_loaded)
            m_backend->unloadShader(handle);

        m_loaded.clear();
    }
}
