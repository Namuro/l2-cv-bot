#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

#include "Input.h"

void Input::MoveMouseSmoothly(const Point &point, Point from, int step, int interval)
{
    const auto distance = std::hypot(point.x - from.x, point.y - from.y);
    const auto steps = distance / step;

    if (steps == 0) {
        return;
    }

    const auto dx = (point.x - from.x) / steps;
    const auto dy = (point.y - from.y) / steps;

    for (int i = 0; i < steps; ++i) {
        MoveMouse({
            static_cast<int>(from.x + i * dx),
            static_cast<int>(from.y + i * dy)
        });

        Delay(interval);
    }

    MoveMouse(point);
}

void Input::PressKeyboardKeyCombination(const std::vector<KeyboardKey> &keys, int delay)
{
    if (keys.empty()) {
        return;
    }

    for (const auto key : keys) {
        KeyboardKeyDown(key);
    }

    Delay(delay);

    for (auto i = keys.size() - 1; i-- > 0;) {
        KeyboardKeyUp(keys[i]);
    }
}

Input::Point Input::MousePosition() const
{
    ::POINT point = {};
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

void Input::AddKeyboardKeyEvent(KeyboardKey key, ::Intercept::KeyboardKeyEvent event)
{
    const auto int_key = static_cast<int>(key);

    if (int_key & SHIFT) {
        KeyboardKeyEvent shift_event = {};
        shift_event.code = KeyScanCode(KeyboardKey::LeftShift);
        shift_event.event = event;
        AddEvent(shift_event);
    }

    KeyboardKeyEvent key_event = {};
    key_event.code = KeyScanCode(key);
    key_event.event = event;
    key_event.e0 = int_key & E0;
    key_event.e1 = int_key & E1;
    AddEvent(key_event);
}

void Input::Send(int sleep)
{
    if (!Ready() || m_events.empty()) {
        return;
    }

    m_ready = false;

    std::thread([this](const decltype(m_events) events, int sleep) { // events copied
        for (const auto &event : events) {
            std::visit([this](const auto &event) {
                using T = std::decay_t<decltype(event)>;

                if constexpr (std::is_same_v<T, MouseMoveEvent>) {
                    m_intercept.SendMouseMoveEvent(event);
                } else if constexpr (std::is_same_v<T, MouseButtonEvent>) {
                    m_intercept.SendMouseButtonEvent(event);
                } else if constexpr (std::is_same_v<T, KeyboardKeyEvent>) {
                    m_intercept.SendKeyboardKeyEvent(event.code, event.event, event.e0, event.e1);
                } else if constexpr (std::is_same_v<T, DelayEvent>) {
                    ::Sleep(event);
                }
            }, event);
        }

        // just sleep for simplicity
        if (sleep > 0) {
            ::Sleep(sleep);
        }

        m_ready = true;
    }, m_events, sleep).detach();

    Reset();
}
