#include <cgame/assets/mapped_file.hpp>

#ifndef _WIN32
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#else
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include <cerrno>
#include <limits>
#include <stdexcept>
#include <system_error>
#include <utility>

namespace cgame::platform
{
#ifndef _WIN32
    mapped_file::mapped_file(const std::filesystem::path& path)
    {
        const int fd = open(path.c_str(), O_RDONLY);
        if (fd == -1)
        {
            throw std::system_error(errno, std::generic_category(),
                                    "failed to open mapped file");
        }
        struct stat info{};
        if (fstat(fd, &info) == -1)
        {
            const int error = errno;
            close(fd);
            throw std::system_error(error, std::generic_category(),
                                    "failed to determine mapped file size");
        }
        if (info.st_size <= 0)
        {
            close(fd);
            throw std::runtime_error("cannot mmap an empty file");
        }
        const auto size = static_cast<std::size_t>(info.st_size);
        void* mapping = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (mapping == MAP_FAILED)
        {
            const int error = errno;
            close(fd);
            throw std::system_error(error, std::generic_category(),
                                    "failed to mmap file");
        }
        m_fileDescriptor = fd;
        m_data = static_cast<const std::byte*>(mapping);
        m_size = size;
    }
#else
    mapped_file::mapped_file(const std::filesystem::path& path)
    {
        HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE)
        {
            throw std::system_error(static_cast<int>(GetLastError()),
                                    std::system_category(), "failed to open mapped file");
        }
        LARGE_INTEGER fileSize{};
        if (!GetFileSizeEx(file, &fileSize))
        {
            const DWORD error = GetLastError();
            CloseHandle(file);
            throw std::system_error(static_cast<int>(error), std::system_category(),
                                    "failed to determine mapped file size");
        }
        if (fileSize.QuadPart <= 0)
        {
            CloseHandle(file);
            throw std::runtime_error("cannot memory-map an empty file");
        }
        const auto size = static_cast<std::size_t>(fileSize.QuadPart);
        HANDLE mapping = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
        if (mapping == nullptr)
        {
            const DWORD error = GetLastError();
            CloseHandle(file);
            throw std::system_error(static_cast<int>(error), std::system_category(),
                                    "failed to create file mapping");
        }
        void* view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
        if (view == nullptr)
        {
            const DWORD error = GetLastError();
            CloseHandle(mapping);
            CloseHandle(file);
            throw std::system_error(static_cast<int>(error), std::system_category(),
                                    "failed to map file");
        }
        m_fileHandle = file;
        m_mappingHandle = mapping;
        m_data = static_cast<const std::byte*>(view);
        m_size = static_cast<std::size_t>(size);
    }
#endif

#ifndef _WIN32
    void mapped_file::reset() noexcept
    {
        if (m_data != nullptr)
        {
            munmap(const_cast<std::byte*>(m_data), m_size);
            m_data = nullptr;
            m_size = 0;
        }
        if (m_fileDescriptor != -1)
        {
            close(m_fileDescriptor);
            m_fileDescriptor = -1;
        }
    }
#else
    void mapped_file::reset() noexcept
    {
        if (m_data != nullptr)
        {
            UnmapViewOfFile(m_data);
            m_data = nullptr;
            m_size = 0;
        }
        if (m_mappingHandle != nullptr)
        {
            CloseHandle(static_cast<HANDLE>(m_mappingHandle));
            m_mappingHandle = nullptr;
        }
        if (m_fileHandle != nullptr)
        {
            CloseHandle(static_cast<HANDLE>(m_fileHandle));
            m_fileHandle = nullptr;
        }
    }
#endif

    mapped_file::mapped_file(mapped_file&& other) noexcept
        : m_data(std::exchange(other.m_data, nullptr)),
          m_size(std::exchange(other.m_size, 0)),
#ifdef _WIN32
          m_fileHandle(std::exchange(other.m_fileHandle, nullptr)),
          m_mappingHandle(std::exchange(other.m_mappingHandle, nullptr))
#else
          m_fileDescriptor(std::exchange(other.m_fileDescriptor, -1))
#endif
    {
    }

    mapped_file& mapped_file::operator=(mapped_file&& other) noexcept
    {
        if (this == &other)
            return *this;
        reset();
        m_data = std::exchange(other.m_data, nullptr);
        m_size = std::exchange(other.m_size, 0);
#ifdef _WIN32
        m_fileHandle = std::exchange(other.m_fileHandle, nullptr);
        m_mappingHandle = std::exchange(other.m_mappingHandle, nullptr);
#else
        m_fileDescriptor = std::exchange(other.m_fileDescriptor, -1);
#endif
        return *this;
    }

    std::span<const std::byte> mapped_file::bytes() const noexcept
    {
        return {m_data, m_size};
    }

    const std::byte* mapped_file::data() const noexcept
    {
        return m_data;
    }

    std::size_t mapped_file::size() const noexcept
    {
        return m_size;
    }
}