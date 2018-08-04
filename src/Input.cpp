#include "Input.h"

void Input::MouseMoveSmoothly(const Point &point, int step, int delay)
{
    const auto from = MousePosition();
    const auto distance = std::hypot(point.x - from.x, point.y - from.y);
    const auto steps = distance / step;
    const auto dx = (point.x - from.x) / steps;
    const auto dy = (point.y - from.y) / steps;

    for (int i = 0; i < steps; ++i) {
        MouseMove({
            static_cast<int>(from.x + i * dx),
            static_cast<int>(from.y + i * dy)
        }, delay);
    }

    MouseMove(point, delay);
}

void Input::Send()
{
    if (!Ready() || m_events.empty()) {
        return;
    }

    m_ready = false;

    std::thread([this](const decltype(m_events) events) {
        for (const auto &pair : events) {
            Event event = pair.first;
            const ::DWORD delay = pair.second;

            if (delay > 0) {
                ::Sleep(delay);
            }

            std::visit([this](auto &&event) {
                using T = std::decay_t<decltype(event)>;

                if constexpr (std::is_same_v<T, ::Intercept::Point>) {
                    m_intercept.SendMouseMoveEvent(event);
                    m_mouse_position = MousePosition();
                } else if constexpr (std::is_same_v<T, ::Intercept::MouseButtonEvent>) {
                    m_intercept.SendMouseButtonEvent(event);
                } else if constexpr (std::is_same_v<T, KeyboardButtonEvent>){
                    m_intercept.SendKeyboardKeyEvent(event.key, event.event);
                }
            }, event);
        }

        m_ready = true;
    }, m_events).detach();

    Reset();
}
