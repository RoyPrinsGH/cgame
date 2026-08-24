#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace cgame::assets
{
    struct virtual_asset_path
    {
        std::vector<std::string> pathParts;
    };

    struct pak_block
    {
        uint64_t offset;
        uint64_t length;
    };

    struct pak_node
    {
        using entry = std::variant<std::unique_ptr<pak_node>, pak_block>;
        std::unordered_map<std::string, entry> contents;
    };

    class pak
    {
      public:
        explicit pak(const std::filesystem::path pakFile)
            : m_pakFile(std::move(pakFile)) {};
        void insertInIndex(const virtual_asset_path& path, pak_block block);
        pak_block findBlock(const virtual_asset_path& path);

      private:
        uint64_t m_cachedMemoryOffset = 0;
        std::vector<std::byte> m_cachedMemory;
        pak_node m_indexRoot{};
        const std::filesystem::path m_pakFile;
    };
}