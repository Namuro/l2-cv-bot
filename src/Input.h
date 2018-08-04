#pragma once

#include <vector>
#include <variant>
#include <mutex>
#include <atomic>

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

#include "Intercept.h"

class Input
{
public:
    struct Point { int x, y; };

    Input(Intercept &intercept) :
        m_intercept     {intercept},
        m_mouse_position{MousePosition()},
        m_ready         {true}
    {}

    Point MousePosition() const
    {
        ::POINT point;
        ::GetCursorPos(&point);
        return {point.x, point.y};
    }

    bool MouseMoved(int delta = 0)
    {
        const auto position = MousePosition();
        std::lock_guard guard(m_mouse_position_mtx);

        return
            std::abs(position.x - m_mouse_position.x) > delta ||
            std::abs(position.y - m_mouse_position.y) > delta;
    }

    void MouseMove(const Point &point, int delay = 0);
    void MouseMoveSmoothly(const Point &point, int step = 30, int delay = 10);
    void MouseLeftDown(int delay = 0);
    void MouseLeftUp(int delay = 0);
    void MouseRightDown(int delay = 0);
    void MouseRightUp(int delay = 0);

    void Send();
    void Reset() { m_events.clear(); }
    bool Ready() const { return m_ready.load(); }

private:
    struct KeyboardButtonEvent
    {
        ::Intercept::KeyboardKeyEvent event;
        ::Intercept::KeyboardKey key;
    };

    using Event = std::variant<::Intercept::Point, ::Intercept::MouseButtonEvent, KeyboardButtonEvent>;

    Intercept &m_intercept;
    std::vector<std::pair<Event, int>> m_events;
    Point m_mouse_position;
    std::mutex m_mouse_position_mtx;
    std::atomic_bool m_ready;

    void AddEvent(Event event, int delay) { if (Ready()) m_events.push_back({event, delay}); }
};
