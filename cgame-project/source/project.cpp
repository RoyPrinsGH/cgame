#include <cgame/project/project.hpp>

#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string_view>

#include <ryml.hpp>

namespace cgame::project
{
    namespace
    {
        constexpr std::string_view PROJECT_DEFINITION_FILE = "project.yaml";

        constexpr std::string_view ASSETS_DIRECTORY = "assets";
        constexpr std::string_view ENTITIES_DIRECTORY = "entities";
        constexpr std::string_view SCENES_DIRECTORY = "scenes";
        constexpr std::string_view SHADERS_DIRECTORY = "shaders";
        constexpr std::string_view SCRIPTS_DIRECTORY = "scripts";

        constexpr std::string_view SHARED_SCRIPTS_DIRECTORY = "shared";
        constexpr std::string_view CLIENT_SCRIPTS_DIRECTORY = "client";
        constexpr std::string_view SERVER_SCRIPTS_DIRECTORY = "server";

        std::string readFileText(const std::filesystem::path& path)
        {
            std::ifstream file(path, std::ios::binary);
            if (!file)
                throw std::runtime_error("could not open " + path.string());
            return {std::istreambuf_iterator<char>(file),
                    std::istreambuf_iterator<char>()};
        }

        ryml::csubstr toCsubstr(const std::string& text)
        {
            return {text.data(), text.size()};
        }

        std::string toString(ryml::csubstr text)
        {
            return text.len > 0 ? std::string{text.str, text.len} : std::string{};
        }

        assets::virtual_asset_path toVirtualPath(const std::filesystem::path& relative)
        {
            std::vector<std::string> parts;
            for (const auto& part : relative.parent_path())
                parts.push_back(part.string());
            parts.push_back(relative.stem().string());
            return assets::virtual_asset_path{std::move(parts)};
        }

        void collectDirectory(const std::filesystem::path& projectRoot,
                              const std::filesystem::path& directory,
                              std::vector<packable_asset>& assets)
        {
            if (!std::filesystem::is_directory(directory))
                return;

            for (const auto& entry :
                 std::filesystem::recursive_directory_iterator(directory))
            {
                if (!entry.is_regular_file())
                    continue;

                const auto relative =
                    std::filesystem::relative(entry.path(), projectRoot);
                assets.push_back({toVirtualPath(relative), entry.path()});
            }
        }
    }

    project_definition project::readProjectDefinition() const
    {
        const auto definitionPath = m_projectRoot / PROJECT_DEFINITION_FILE;
        const auto definitionPathText = definitionPath.string();
        const auto yaml = readFileText(definitionPath);

        const ryml::Tree tree =
            ryml::parse_in_arena(toCsubstr(definitionPathText), toCsubstr(yaml));
        const ryml::ConstNodeRef root = tree.rootref();

        if (!root.readable() || !root.is_map())
            throw std::runtime_error(definitionPathText + " is not a yaml map");

        project_definition definition;

        if (root.has_child("name"))
            definition.name = toString(root["name"].val());

        if (root.has_child("buildMultiplayer"))
        {
            const auto value = root["buildMultiplayer"].val();
            if (!ryml::from_chars(value, &definition.buildMultiplayer))
                throw std::runtime_error(
                    definitionPathText +
                    ": buildMultiplayer is not a boolean: " + toString(value));
        }

        return definition;
    }

    std::vector<packable_asset> project::getAssetsForPacking(pack_mode mode) const
    {
        std::vector<packable_asset> assets;

        collectDirectory(m_projectRoot, m_projectRoot / ASSETS_DIRECTORY, assets);
        collectDirectory(m_projectRoot, m_projectRoot / ENTITIES_DIRECTORY, assets);
        collectDirectory(m_projectRoot, m_projectRoot / SCENES_DIRECTORY, assets);

        const auto scripts = m_projectRoot / SCRIPTS_DIRECTORY;
        const auto modeScripts = mode == pack_mode::client ? CLIENT_SCRIPTS_DIRECTORY
                                                           : SERVER_SCRIPTS_DIRECTORY;

        collectDirectory(m_projectRoot, scripts / SHARED_SCRIPTS_DIRECTORY, assets);
        collectDirectory(m_projectRoot, scripts / modeScripts, assets);

        if (mode == pack_mode::client)
            collectDirectory(m_projectRoot, m_projectRoot / SHADERS_DIRECTORY, assets);

        return assets;
    }
}
