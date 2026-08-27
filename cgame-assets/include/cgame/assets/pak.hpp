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
    class virtual_asset_path
    {
      public:
        virtual_asset_path(std::vector<std::string> pathParts)
            : m_pathParts(std::move(pathParts)) {};
        [[nodiscard]]
        std::span<const std::string> pathParts() const
        {
            return {m_pathParts.begin(), m_pathParts.size()};
        }

      private:
        std::vector<std::string> m_pathParts;
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

    struct pak_index_entry
    {
        std::uint16_t pathLength;
        char path[256];
        // to make it 8 byte-aligned
        std::uint8_t reserved[6]{};
        std::uint64_t offset;
        std::uint64_t length;
    };

    static_assert(sizeof(pak_index_entry) == 280);

    class pak
    {
      public:
        explicit pak(std::filesystem::path pakFile) : m_pakFile(pakFile)
        {
            loadIndex(pakFile += ".index");
        };
        std::span<const std::byte> data(const virtual_asset_path& path) const;

      private:
        pak_block findBlock(const virtual_asset_path& path) const;
        void loadIndex(const std::filesystem::path& indexPath);
        void insertInIndex(const virtual_asset_path& path, pak_block block);
        pak_node m_indexRoot{};
        cgame::platform::mapped_file m_pakFile;
    };

    class pak_builder
    {
      public:
        void add(const virtual_asset_path& path, const std::filesystem::path& realFile);
        void build(const std::filesystem::path& outputPakPath);

      private:
        struct pending_entry
        {
            std::string path;
            std::filesystem::path realFile;
        };
        std::vector<pending_entry> m_entries;
    };
}