#pragma once

#include <vector>
#include <variant>
#include <atomic>

#include "Intercept.h"

class Input
{
    static constexpr auto SHIFT = 0x0800;
    static constexpr auto E0    = SHIFT << 1;
    static constexpr auto E1    = SHIFT << 2;

public:
    // QWERTY layout
    enum class KeyboardKey
    {
        Escape              = 0x01,
        One                 = 0x02,
        ExclamationPoint    = 0x02 | SHIFT,
        Two                 = 0x03,
        At                  = 0x03 | SHIFT,
        Three               = 0x04,
        Hash                = 0x04 | SHIFT,
        Four                = 0x05,
        Dollar              = 0x05 | SHIFT,
        Five                = 0x06,
        Percent             = 0x06 | SHIFT,
        Six                 = 0x07,
        Caret               = 0x07 | SHIFT,
        Seven               = 0x08,
        Ampersand           = 0x08 | SHIFT,
        Eight               = 0x09,
        Asterisk            = 0x09 | SHIFT,
        Nine                = 0x0A,
        OpenParenthesis     = 0x0A | SHIFT,
        Zero                = 0x0B,
        CloseParenthesis    = 0x0B | SHIFT,
        Minus               = 0x0C,
        Underscore          = 0x0C | SHIFT,
        Equals              = 0x0D,
        Plus                = 0x0D | SHIFT,
        Backspace           = 0x0E,
        Tab                 = 0x0F,
        Q                   = 0x10,
        W                   = 0x11,
        E                   = 0x12,
        R                   = 0x13,
        T                   = 0x14,
        Y                   = 0x15,
        U                   = 0x16,
        I                   = 0x17,
        O                   = 0x18,
        P                   = 0x19,
        OpenBracket         = 0x1A,
        OpenBrace           = 0x1A | SHIFT,
        CloseBracket        = 0x1B,
        CloseBrace          = 0x1B | SHIFT,
        Enter               = 0x1C,
        LeftCtrl            = 0x1D,
        RightCtrl           = 0x1D | E0,
        A                   = 0x1E,
        S                   = 0x1F,
        D                   = 0x20,
        F                   = 0x21,
        G                   = 0x22,
        H                   = 0x23,
        J                   = 0x24,
        K                   = 0x25,
        L                   = 0x26,
        Semicolon           = 0x27,
        Colon               = 0x27 | SHIFT,
        SingleQuote         = 0x28,
        DoubleQuote         = 0x28 | SHIFT,
        Grave               = 0x29,
        Tilde               = 0x29 | SHIFT,
        LeftShift           = 0x2A,
        Backslash           = 0x2B,
        Pipe                = 0x2B | SHIFT,
        Z                   = 0x2C,
        X                   = 0x2D,
        C                   = 0x2E,
        V                   = 0x2F,
        B                   = 0x30,
        N                   = 0x31,
        M                   = 0x32,
        Comma               = 0x33,
        LessThan            = 0x33 | SHIFT,
        Period              = 0x34,
        GreaterThan         = 0x34 | SHIFT,
        Slash               = 0x35,
        QuestionMark        = 0x35 | SHIFT,
        RightShift          = 0x36,
        NumAsterisk         = 0x37,
        PrintScreen         = 0x37 | E0,
        LeftAlt             = 0x38,
        RightAlt            = 0x38 | E0,
        Space               = 0x39,
        CapsLock            = 0x3A,
        F1                  = 0x3B,
        F2                  = 0x3C,
        F3                  = 0x3D,
        F4                  = 0x3E,
        F5                  = 0x3F,
        F6                  = 0x40,
        F7                  = 0x41,
        F8                  = 0x42,
        F9                  = 0x43,
        F10                 = 0x44,
        NumLock             = 0x45,
        ScrollLock          = 0x46,
        Num7                = 0x47,
        Home                = 0x47 | E0,
        Num8                = 0x48,
        Up                  = 0x48 | E0,
        Num9                = 0x49,
        PageUp              = 0x49 | E0,
        NumMinus            = 0x4A,
        Num4                = 0x4B,
        Left                = 0x4B | E0,
        Num5                = 0x4C,
        Num6                = 0x4D,
        Right               = 0x4D | E0,
        NumPlus             = 0x4E,
        Num1                = 0x4F,
        End                 = 0x4F | E0,
        Num2                = 0x50,
        Down                = 0x50 | E0,
        Num3                = 0x51,
        PageDown            = 0x51 | E0,
        Num0                = 0x52,
        Insert              = 0x52 | E0,
        NumPeriod           = 0x53,
        Delete              = 0x53 | E0,
        F11                 = 0x57,
        F12                 = 0x58,
        System              = 0x5B | E0,

        Max                 = ::Intercept::KEYBOARD_KEY_MAX // 0xFF
    };

    struct Point { int x, y; };

    Input() : // throws InterceptionDriverNotFoundError
        m_intercept     {},
        m_mouse_position{MousePosition()},
        m_ready         {true}
    {}

    void MoveMouse(const Point &point)
        { AddEvent(MouseMoveEvent{point.x, point.y}); m_mouse_position = point; }

    void MoveMouseSmoothly(const Point &point, Point from, int step = 20, int interval = 10);
    void MoveMouseSmoothly(const Point &point) { MoveMouseSmoothly(point, m_mouse_position); }

    void MoveMouseSmoothlyBy(const Point &point)
        { MoveMouseSmoothly({m_mouse_position.x + point.x, m_mouse_position.y + point.y}); }

    void LeftMouseButtonClick(int delay = 50)
        { LeftMouseButtonDown(); Delay(delay); LeftMouseButtonUp(); }

    void RightMouseButtonClick(int delay = 50)
        { RightMouseButtonDown(); Delay(delay); RightMouseButtonUp(); }

    void LeftMouseButtonDoubleClick(int delay = 50)
        { LeftMouseButtonClick(delay); Delay(delay); LeftMouseButtonClick(delay); }

    void RightMouseButtonDoubleClick(int delay = 50)
        { RightMouseButtonClick(delay); Delay(delay); RightMouseButtonClick(delay); }

    void LeftMouseButtonDown()
        { AddMouseButtonEvent(::Intercept::MouseButtonEvent::LeftDown); }

    void LeftMouseButtonUp()
        { AddMouseButtonEvent(::Intercept::MouseButtonEvent::LeftUp); }

    void RightMouseButtonDown()
        { AddMouseButtonEvent(::Intercept::MouseButtonEvent::RightDown); }

    void RightMouseButtonUp()
        { AddMouseButtonEvent(::Intercept::MouseButtonEvent::RightUp); }

    void PressKeyboardKey(KeyboardKey key, int delay = 50)
        { KeyboardKeyDown(key); Delay(delay); KeyboardKeyUp(key); }

    void PressKeyboardKeyCombination(const std::vector<KeyboardKey> &keys, int delay = 50);

    void KeyboardKeyDown(KeyboardKey key)
        { AddKeyboardKeyEvent(key, ::Intercept::KeyboardKeyEvent::Down); }

    void KeyboardKeyUp(KeyboardKey key)
        { AddKeyboardKeyEvent(key, ::Intercept::KeyboardKeyEvent::Up); }

    void Delay(int delay) { AddEvent(DelayEvent{delay}); }
    
    Point MousePosition() const;
    bool MouseMoved(int delta = 0);
    bool LeftMouseButtonPressed()   { return m_intercept.MouseButtonPressed(::Intercept::MouseButton::Left); }
    bool RightMouseButtonPressed()  { return m_intercept.MouseButtonPressed(::Intercept::MouseButton::Right); }
    bool KeyboardKeyPressed(KeyboardKey key);

    void Send(int sleep = 0);
    void Reset() { m_events.clear(); }
    bool Ready() const { return m_ready.load(); }

private:
    using MouseMoveEvent = ::Intercept::Point;
    using MouseButtonEvent = ::Intercept::MouseButtonEvent;
    using DelayEvent = int;

    struct KeyboardKeyEvent
    {
        int code;
        ::Intercept::KeyboardKeyEvent event;
        bool e0;
        bool e1;
    };

    using Event = std::variant<MouseMoveEvent, MouseButtonEvent, KeyboardKeyEvent, DelayEvent>;

    ::Intercept m_intercept;
    std::vector<Event> m_events;
    Point m_mouse_position;
    std::atomic_bool m_ready;

    void AddMouseButtonEvent(::Intercept::MouseButtonEvent event)
        { AddEvent(MouseButtonEvent{event}); }

    void AddKeyboardKeyEvent(KeyboardKey key, ::Intercept::KeyboardKeyEvent event);
    void AddEvent(Event event) { if (Ready()) m_events.push_back(event); }

    static int KeyScanCode(KeyboardKey key) { return static_cast<int>(key) & ~(SHIFT | E0 | E1); }
};
