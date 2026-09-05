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
        const std::string key = keyOf(vertexPath, fragmentPath);

        if (const auto found = m_loaded.find(key); found != m_loaded.end())
            return found->second;

        const shader_handle handle = m_backend->loadShader(
            asText(m_pak->data(vertexPath)), asText(m_pak->data(fragmentPath)));

        m_loaded.emplace(key, handle);

        return handle;
    }

    std::string shader_controller::keyOf(const assets::virtual_asset_path& vertexPath,
                                         const assets::virtual_asset_path& fragmentPath)
    {
        std::string key;

        for (const std::string& part : vertexPath.pathParts())
        {
            key += part;
            key += '/';
        }

        key += '\n';

        for (const std::string& part : fragmentPath.pathParts())
        {
            key += part;
            key += '/';
        }

        return key;
    }
}
