#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <cgame/assets/mapped_file.hpp>

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
        explicit pak(const std::filesystem::path pakFile) : m_file(std::move(pakFile)) {};
        void insertInIndex(const virtual_asset_path& path, pak_block block);
        std::span<const std::byte> data(const virtual_asset_path& path) const;

      private:
        pak_block findBlock(const virtual_asset_path& path) const;
        cgame::platform::mapped_file m_file;
        pak_node m_indexRoot{};
    };
}