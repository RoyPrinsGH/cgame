#pragma once
#include <array>
#include <atomic>
#include <optional>

template <typename TItem, size_t Capacity>
class SpscQueue
{
public:
    bool push(TItem value)
    {
        const auto head = m_head.load(std::memory_order_relaxed);
        const auto next = (head + 1) % Capacity;

        if (next == m_tail.load(std::memory_order_acquire))
            return false;

        m_buffer[head] = std::move(value);

        m_head.store(next, std::memory_order_release);
        return true;
    }

    std::optional<TItem> pop()
    {
        const auto tail = m_tail.load(std::memory_order_relaxed);

        if (tail == m_head.load(std::memory_order_acquire))
            return std::nullopt;

        TItem value = std::move(m_buffer[tail]);

        m_tail.store((tail + 1) % Capacity, std::memory_order_release);

        return value;
    }

private:
    std::array<TItem, Capacity> m_buffer;
    std::atomic<std::size_t> m_head{0};
    std::atomic<std::size_t> m_tail{0};
};