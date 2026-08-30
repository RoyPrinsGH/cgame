#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <cgame/assets/pak.hpp>

namespace cgame::project
{
    struct project_definition
    {
        std::string name;
        bool buildMultiplayer = false;
    };

    enum class pack_mode
    {
        client,
        server,
    };

    struct packable_asset
    {
        assets::virtual_asset_path virtualPath;
        std::filesystem::path realFile;
    };

    class project
    {
      public:
        explicit project(std::filesystem::path projectRoot)
            : m_projectRoot(std::move(projectRoot)) {};

        [[nodiscard]]
        const std::filesystem::path& root() const noexcept
        {
            return m_projectRoot;
        }

        [[nodiscard]]
        project_definition readProjectDefinition() const;

        [[nodiscard]]
        std::vector<packable_asset> getAssetsForPacking(pack_mode mode) const;

      private:
        std::filesystem::path m_projectRoot;
    };
}
