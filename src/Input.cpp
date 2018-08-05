#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

#include "Input.h"

void Input::MoveMouse(const Point &point, int delay)
{
    MouseMoveEvent move_event = {};
    move_event.position = {point.x, point.y};
    AddEvent(move_event, delay);
    m_mouse_position = point;
}

void Input::MoveMouseSmoothly(const Point &point, const Point &from, int step, int delay, int interval)
{
    const auto distance = std::hypot(point.x - from.x, point.y - from.y);
    const auto steps = distance / step;
    const auto dx = (point.x - from.x) / steps;
    const auto dy = (point.y - from.y) / steps;

    for (int i = 0; i < steps; ++i) {
        MoveMouse({
            static_cast<int>(from.x + i * dx),
            static_cast<int>(from.y + i * dy)
        }, i > 0 ? interval : delay);
    }

    MoveMouse(point, steps > 0 ? interval : delay);
}

Input::Point Input::MousePosition() const
{
    ::POINT point;
    ::GetCursorPos(&point);
    return {point.x, point.y};
}

bool Input::MouseMoved(int delta)
{
    const auto mouse_delta = m_intercept.MouseDelta();
    return std::abs(mouse_delta.x) > delta || std::abs(mouse_delta.y) > delta;
}

bool Input::KeyboardKeyPressed(KeyboardKey key)
{
    const auto code = KeyScanCode(key);

    if (static_cast<int>(key) & SHIFT) {
        const auto lshift = static_cast<int>(KeyboardKey::LeftShift);
        const auto rshift = static_cast<int>(KeyboardKey::RightShift);

        return m_intercept.KeyboardKeyPressed(code) && (
            m_intercept.KeyboardKeyPressed(lshift) ||
            m_intercept.KeyboardKeyPressed(rshift)
        );
    } else {
        return m_intercept.KeyboardKeyPressed(code);
    }
}

void Input::AddMouseButtonEvent(::Intercept::MouseButtonEvent event, int delay)
{
    MouseButtonEvent button_event = {};
    button_event.event = event;
    AddEvent(button_event, delay);
}

void Input::AddKeyboardKeyEvent(KeyboardKey key, ::Intercept::KeyboardKeyEvent event, int delay)
{
    const auto int_key = static_cast<int>(key);

    if (int_key & SHIFT) {
        KeyboardKeyEvent shift_event = {};
        shift_event.code = KeyScanCode(KeyboardKey::LeftShift);
        shift_event.event = event;
        AddEvent(shift_event, delay);
        delay = 0;
    }

    KeyboardKeyEvent key_event = {};
    key_event.code = KeyScanCode(key);
    key_event.event = event;
    key_event.e0 = int_key & E0;
    key_event.e1 = int_key & E1;
    AddEvent(key_event, delay);
}

void Input::Send()
{
    if (!Ready() || m_events.empty()) {
        return;
    }

    m_ready = false;

    std::thread([this](const decltype(m_events) events) {
        for (const auto &pair : events) {
            const auto [event, delay] = pair;

            if (delay > 0) {
                ::Sleep(delay);
            }

            std::visit([this](auto &&event) {
                using T = std::decay_t<decltype(event)>;

                if constexpr (std::is_same_v<T, MouseMoveEvent>) {
                    m_intercept.SendMouseMoveEvent(event.position);
                } else if constexpr (std::is_same_v<T, MouseButtonEvent>) {
                    m_intercept.SendMouseButtonEvent(event.event);
                } else if constexpr (std::is_same_v<T, KeyboardKeyEvent>) {
                    m_intercept.SendKeyboardKeyEvent(event.code, event.event, event.e0, event.e1);
                }
            }, event);
        }

        m_ready = true;
    }, m_events).detach();

    Reset();
}
