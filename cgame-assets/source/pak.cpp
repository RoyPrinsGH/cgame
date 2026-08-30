#include <cgame/assets/pak.hpp>

#include <algorithm>
#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>

namespace cgame::assets
{
    void pak::insertInIndex(const virtual_asset_path& path, pak_block block)
    {
        if (path.pathParts().empty())
            throw std::invalid_argument("path cannot be empty");
        pak_node* currentNode = &m_indexRoot;
        for (std::size_t i = 0; i + 1 < path.pathParts().size(); i++)
        {
            const auto& pathPart = path.pathParts()[i];
            auto it = currentNode->contents.find(pathPart);
            if (it == currentNode->contents.end())
            {
                auto [newIt, inserted] =
                    currentNode->contents.emplace(pathPart, std::make_unique<pak_node>());
                it = newIt;
            }
            auto* node = std::get_if<std::unique_ptr<pak_node>>(&it->second);
            if (node == nullptr || *node == nullptr)
                throw std::invalid_argument("path component is not a directory: " +
                                            pathPart);
            currentNode = node->get();
        }
        const auto& filetag = path.pathParts().back();
        auto [it, inserted] =
            currentNode->contents.try_emplace(filetag, std::move(block));
        if (!inserted)
            throw std::invalid_argument("path already exists: " + filetag);
    }

    // TODO: maybe make optional?
    pak_block pak::findBlock(const virtual_asset_path& path) const
    {
        if (path.pathParts().empty())
            throw std::invalid_argument("path cannot be empty");
        const pak_node* currentNode = &m_indexRoot;
        for (std::size_t i = 0; i + 1 < path.pathParts().size(); i++)
        {
            const auto& pathPart = path.pathParts()[i];
            auto it = currentNode->contents.find(pathPart);
            if (it == currentNode->contents.end())
                throw std::invalid_argument("path does not exist: " + pathPart);
            auto* node = std::get_if<std::unique_ptr<pak_node>>(&it->second);
            if (node == nullptr || *node == nullptr)
                throw std::invalid_argument("path component is not a directory: " +
                                            pathPart);
            currentNode = node->get();
        }
        const auto& filetag = path.pathParts().back();
        auto it = currentNode->contents.find(filetag);
        if (it == currentNode->contents.end())
            throw std::invalid_argument("path does not exist: " + filetag);
        auto* block = std::get_if<pak_block>(&it->second);
        if (block == nullptr)
            throw std::invalid_argument("path component is not a block: " + filetag);
        return *block;
    }

    virtual_asset_path deserializeVirtualAssetPath(std::string_view path)
    {
        std::vector<std::string> parts;
        std::size_t start = 0;
        while (start < path.size())
        {
            const auto end = path.find('/', start);
            if (end == std::string_view::npos)
            {
                parts.emplace_back(path.substr(start));
                break;
            }
            parts.emplace_back(path.substr(start, end - start));
            start = end + 1;
        }
        return virtual_asset_path{std::move(parts)};
    }

    void pak::loadIndex(const std::filesystem::path& indexPath)
    {
        std::ifstream file(indexPath, std::ios::binary | std::ios::ate);
        if (!file)
            throw std::runtime_error("failed to open pak index");
        const auto fileSize = static_cast<std::uint64_t>(file.tellg());
        if (fileSize < 0 || fileSize % sizeof(pak_index_entry) != 0)
        {
            throw std::runtime_error("invalid pak index");
        }
        const auto entryCount = fileSize / sizeof(pak_index_entry);
        file.seekg(0);
        for (std::uint64_t i = 0; i < entryCount; ++i)
        {
            pak_index_entry entry{};
            file.read(reinterpret_cast<char*>(&entry), sizeof(entry));
            if (!file)
                throw std::runtime_error("failed to read pak index");
            if (entry.pathLength > 256)
                throw std::runtime_error("invalid path length");
            const std::string_view serializedPath{entry.path, entry.pathLength};
            auto path = deserializeVirtualAssetPath(serializedPath);
            const pak_block block{
                .offset = entry.offset,
                .length = entry.length,
            };
            insertInIndex(path, block);
        }
    }

    std::span<const std::byte> pak::data(const virtual_asset_path& path) const
    {
        const pak_block block = findBlock(path);
        return m_pakFile.bytes().subspan(block.offset, block.length);
    }

    std::string serializeVirtualAssetPath(const virtual_asset_path& path)
    {
        std::string result;
        bool first = true;
        for (const auto& part : path.pathParts())
        {
            if (!first)
                result += '/';
            result += part;
            first = false;
        }
        return result;
    }

    void pak_builder::add(const virtual_asset_path& path,
                          const std::filesystem::path& realFile)
    {
        auto serializedPath = serializeVirtualAssetPath(path);
        if (serializedPath.size() > 256)
            throw std::runtime_error("virtual asset path exceeds 256 bytes");
        m_entries.push_back({
            .path = std::move(serializedPath),
            .realFile = realFile,
        });
    }

    void pak_builder::build(const std::filesystem::path& outputPakPath)
    {
        std::ranges::sort(m_entries, {}, &pending_entry::path);
        for (std::size_t i = 1; i < m_entries.size(); ++i)
        {
            if (m_entries[i - 1].path == m_entries[i].path)
                throw std::runtime_error("duplicate virtual asset path: " +
                                         m_entries[i].path);
        }
        auto outputIndexPath = outputPakPath;
        outputIndexPath += ".index";
        std::ofstream pakFile(outputPakPath, std::ios::binary | std::ios::trunc);
        if (!pakFile)
            throw std::runtime_error("failed to create pak file");
        std::ofstream indexFile(outputIndexPath, std::ios::binary | std::ios::trunc);
        if (!indexFile)
            throw std::runtime_error("failed to create pak index");
        std::uint64_t currentOffset = 0;
        for (const auto& pending : m_entries)
        {
            std::ifstream inputFile(pending.realFile, std::ios::binary);
            if (!inputFile)
                throw std::runtime_error("failed to open asset: " +
                                         pending.realFile.string());
            const auto fileSize = std::filesystem::file_size(pending.realFile);
            pak_index_entry indexEntry{};
            indexEntry.pathLength = static_cast<std::uint16_t>(pending.path.size());
            std::memcpy(indexEntry.path, pending.path.data(), pending.path.size());
            indexEntry.offset = currentOffset;
            indexEntry.length = fileSize;
            indexFile.write(reinterpret_cast<const char*>(&indexEntry),
                            sizeof(indexEntry));
            indexFile.flush();
            if (!indexFile)
                throw std::runtime_error("failed to write pak index");
            if (fileSize > 0)
            {
                pakFile << inputFile.rdbuf();
                if (!pakFile)
                    throw std::runtime_error("failed while writing pak: " +
                                             pending.realFile.string());
            }
            else
            {
                std::cerr << "[ERR] virtual asset \"" << pending.path
                          << "\" is 0 bytes -- skipping" << std::endl;
            }
            currentOffset += fileSize;
        }
    }
}