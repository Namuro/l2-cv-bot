#pragma once

#include <memory>
#include <array>
#include <mutex>

#include "interception.h"

class Intercept
{
public:
    struct InterceptionDriverNotFoundError : public std::runtime_error
        { InterceptionDriverNotFoundError() : std::runtime_error("Interception driver not found") {} };

    static constexpr size_t KEYBOARD_KEY_MAX = 256;

    enum class KeyboardKeyEvent : unsigned short
    {
        Down    = INTERCEPTION_KEY_DOWN,
        Up      = INTERCEPTION_KEY_UP
    };

    enum class MouseButtonEvent : unsigned short
    {
        LeftDown    = INTERCEPTION_MOUSE_LEFT_BUTTON_DOWN,
        LeftUp      = INTERCEPTION_MOUSE_LEFT_BUTTON_UP,
        RightDown   = INTERCEPTION_MOUSE_RIGHT_BUTTON_DOWN,
        RightUp     = INTERCEPTION_MOUSE_RIGHT_BUTTON_UP
    };

    enum class MouseButton
    {
        Left    = 1,
        Right   = 2,
        Middle  = 3,
        Fourth  = 4,
        Fifth   = 5,

        Max     = 6
    };

    struct Point { int x, y; };

    Intercept(); // throws

    void SendMouseMoveEvent(const Point &point) const;
    void SendMouseButtonEvent(MouseButtonEvent event) const;
    void SendKeyboardKeyEvent(int code, KeyboardKeyEvent event, bool e0, bool e1) const;

    bool MouseButtonPressed(MouseButton button);
    bool KeyboardKeyPressed(int code);

    Point MouseDelta();

private:
    struct InterceptionContextDestroyer
    {
        using pointer = ::InterceptionContext;
        void operator()(::InterceptionContext context) const { ::interception_destroy_context(context); }
    };

    int m_screen_width, m_screen_height;
    std::unique_ptr<::InterceptionContext, InterceptionContextDestroyer> m_context;
    ::InterceptionDevice m_keyboard_device;
    ::InterceptionDevice m_mouse_device;
    std::array<bool, KEYBOARD_KEY_MAX> m_pressed_keyboard_keys;
    std::array<bool, static_cast<size_t>(MouseButton::Max)> m_pressed_mouse_buttons;
    Point m_mouse_delta;
    std::mutex m_keyboard_mtx;
    std::mutex m_mouse_mtx;
};
