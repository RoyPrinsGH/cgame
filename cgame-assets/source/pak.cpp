#include <cgame/assets/pak.hpp>

namespace cgame::assets
{
    void pak::insertInIndex(const virtual_asset_path& path, pak_block block)
    {
        if (path.pathParts.empty())
            throw std::invalid_argument("path cannot be empty");
        pak_node* currentNode = &m_indexRoot;
        for (std::size_t i = 0; i + 1 < path.pathParts.size(); i++)
        {
            const auto& pathPart = path.pathParts[i];
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
        const auto& filetag = path.pathParts.back();
        auto [it, inserted] =
            currentNode->contents.try_emplace(filetag, std::move(block));
        if (!inserted)
            throw std::invalid_argument("path already exists: " + filetag);
    }

    // TODO: maybe make optional?
    pak_block pak::findBlock(const virtual_asset_path& path) const
    {
        if (path.pathParts.empty())
            throw std::invalid_argument("path cannot be empty");
        const pak_node* currentNode = &m_indexRoot;
        for (std::size_t i = 0; i + 1 < path.pathParts.size(); i++)
        {
            const auto& pathPart = path.pathParts[i];
            auto it = currentNode->contents.find(pathPart);
            if (it == currentNode->contents.end())
                throw std::invalid_argument("path does not exist: " + pathPart);
            auto* node = std::get_if<std::unique_ptr<pak_node>>(&it->second);
            if (node == nullptr || *node == nullptr)
                throw std::invalid_argument("path component is not a directory: " +
                                            pathPart);
            currentNode = node->get();
        }
        const auto& filetag = path.pathParts.back();
        auto it = currentNode->contents.find(filetag);
        if (it == currentNode->contents.end())
            throw std::invalid_argument("path does not exist: " + filetag);
        auto* block = std::get_if<pak_block>(&it->second);
        if (block == nullptr)
            throw std::invalid_argument("path component is not a block: " + filetag);
        return *block;
    }

    std::span<const std::byte> pak::data(const virtual_asset_path& path) const
    {
        const pak_block block = findBlock(path);
        return m_file.bytes().subspan(block.offset, block.length);
    }
}