#pragma once
#include <boost/circular_buffer.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <unordered_map>
#include <vector>
#include <cgame/platform/input/raw_input_event.hpp>
#include <glm/vec3.hpp>

namespace engine::events
{
    namespace camera
    {
        struct camera_move_event
        {
            glm::vec3 delta;
        };

        using camera_event = std::variant<camera_move_event>;
    };

    struct tick_events_unbounded
    {
        std::vector<cgame::platform::input::raw::input_event> m_inputEvents;
        std::vector<camera::camera_event> m_cameraEvents;
    };

    class tick_history
    {
    public:
        tick_history() : m_tickHistory(1024) {}
        void registerHistory(int tick, const tick_events_unbounded tickEvents) { m_tickHistory.push_front({tick, std::move(tickEvents)}); }
        auto getHistoryFrom(int tick)
        {
            auto tickFilter = [tick](const auto &p)
            { return p.first > tick; };
            auto begin = boost::make_filter_iterator(tickFilter, m_tickHistory.begin(), m_tickHistory.end());
            auto end = boost::make_filter_iterator(tickFilter, m_tickHistory.end(), m_tickHistory.end());
            return boost::make_iterator_range(begin, end);
        }

    private:
        boost::circular_buffer<std::pair<int, tick_events_unbounded>> m_tickHistory;
    };
}