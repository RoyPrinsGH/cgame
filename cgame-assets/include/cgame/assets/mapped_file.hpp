#pragma once

#include <cstddef>
#include <filesystem>
#include <span>

namespace cgame::platform
{
    class mapped_file
    {
      public:
        explicit mapped_file(const std::filesystem::path& path);
        ~mapped_file()
        {
            reset();
        }

        mapped_file(const mapped_file&) = delete;
        mapped_file& operator=(const mapped_file&) = delete;

        mapped_file(mapped_file&& other) noexcept;
        mapped_file& operator=(mapped_file&& other) noexcept;

        [[nodiscard]]
        std::span<const std::byte> bytes() const noexcept;

        [[nodiscard]]
        const std::byte* data() const noexcept;

        [[nodiscard]]
        std::size_t size() const noexcept;

      private:
        void reset() noexcept;

        const std::byte* m_data = nullptr;
        std::size_t m_size = 0;

#ifndef _WIN32
        int m_fileDescriptor = -1;
#else
        void* m_fileHandle = nullptr;
        void* m_mappingHandle = nullptr;
#endif
    };
}