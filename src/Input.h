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
        //ShiftF1             = 0x54,
        //ShiftF2             = 0x55,
        //ShiftF3             = 0x56,
        F11                 = 0x57,
        F12                 = 0x58,
        //ShiftF6             = 0x59,
        //ShiftF7             = 0x5A,
        //ShiftF8             = 0x5B,
        System              = 0x5B | E0,
        //ShiftF9             = 0x5C,
        //ShiftF10            = 0x5D,
        //CtrlF1              = 0x5E,
        //CtrlF2              = 0x5F,
        //CtrlF3              = 0x60,
        //CtrlF4              = 0x61,
        //CtrlF5              = 0x62,
        //CtrlF6              = 0x63,
        //CtrlF7              = 0x64,
        //CtrlF8              = 0x65,
        //CtrlF9              = 0x66,
        //CtrlF10             = 0x67,
        //AltF1               = 0x68,
        //AltF2               = 0x69,
        //AltF3               = 0x6A,
        //AltF4               = 0x6B,
        //AltF5               = 0x6C,
        //AltF6               = 0x6D,
        //AltF7               = 0x6E,
        //AltF8               = 0x6F,
        //AltF9               = 0x70,
        //AltF10              = 0x71,
        //CtrlPrintScreen     = 0x72,
        //CtrlLeft            = 0x73,
        //CtrlRight           = 0x74,
        //CtrlEnd             = 0x75,
        //CtrlPageDown        = 0x76,
        //CtrlHome            = 0x77,
        //Alt1                = 0x78,
        //Alt2                = 0x79,
        //Alt3                = 0x7A,
        //Alt4                = 0x7B,
        //Alt5                = 0x7C,
        //Alt6                = 0x7D,
        //Alt7                = 0x7E,
        //Alt8                = 0x7F,
        //Alt9                = 0x80,
        //Alt10               = 0x81,
        //AltMinus            = 0x82,
        //AltEquals           = 0x83,
        //CtrlPageUp          = 0x84,
        ////                    0x85
        ////                    0x86
        //ShiftF11            = 0x87,
        //ShiftF12            = 0x88,
        //CtrlF11             = 0x89,
        //CtrlF12             = 0x8A,
        //AltF11              = 0x8B,
        //AltF12              = 0x8C,
        //CtrlUp              = 0x8D,
        //CtrlNumMinus        = 0x8E,
        //CtrlNum5            = 0x8F,
        //CtrlNumPlus         = 0x90,
        //CtrlNumDown         = 0x91,
        //CtrlInsert          = 0x92,
        //CtrlDelete          = 0x93,
        //CtrlTab             = 0x94,
        //CtrlNumSlash        = 0x95,
        //CtrlNumAsterisk     = 0x96,
        //AltHome             = 0x97,
        //AltUp               = 0x98,
        //AltPageUp           = 0x99,
        ////                    0x9A
        //AltLeft             = 0x9B,
        ////                    0x9C
        //AltRight            = 0x9D,
        ////                    0x9E
        //AltEnd              = 0x9F,
        //AltDown             = 0xA0,
        //AltPageDown         = 0xA1,
        //AltInsert           = 0xA2,
        //AltDelete           = 0xA3,
        //AltNumSlash         = 0xA4,
        //AltTab              = 0xA5,
        //AltNumEnter         = 0xA6,

        Max                 = ::Intercept::KEYBOARD_KEY_MAX // 0xFF
    };

    struct Point { int x, y; };

    Input() : m_intercept{}, m_mouse_position{MousePosition()}, m_ready{true} {} // throws

    void MoveMouse(const Point &point, int delay = 0);
    void MoveMouseSmoothly(const Point &point, const Point &from, int step = 20, int delay = 0, int interval = 10);

    void MoveMouseSmoothly(const Point &point, bool continues = true, int step = 20, int delay = 0,int interval = 10)
        { MoveMouseSmoothly(point, continues ? m_mouse_position : MousePosition(), step, delay, interval); }

    void PressLeftMouseButton(int delay = 0, int interval = 50)
        { MouseLeftButtonDown(delay); MouseLeftButtonUp(interval); }

    void PressRightMouseButton(int delay = 0, int interval = 50)
        { MouseRightButtonDown(delay); MouseRightButtonUp(interval); }

    void MouseLeftButtonDown(int delay = 0)
        { AddMouseButtonEvent(::Intercept::MouseButtonEvent::LeftDown, delay); }

    void MouseLeftButtonUp(int delay = 0)
        { AddMouseButtonEvent(::Intercept::MouseButtonEvent::LeftUp, delay); }

    void MouseRightButtonDown(int delay = 0)
        { AddMouseButtonEvent(::Intercept::MouseButtonEvent::RightDown, delay); }

    void MouseRightButtonUp(int delay = 0)
        { AddMouseButtonEvent(::Intercept::MouseButtonEvent::RightUp, delay); }

    void PressKeyboardKey(KeyboardKey key, int delay = 0, int interval = 50)
        { KeyboardKeyDown(key, delay); KeyboardKeyUp(key, interval); }

    void KeyboardKeyDown(KeyboardKey key, int delay = 0)
        { AddKeyboardKeyEvent(key, ::Intercept::KeyboardKeyEvent::Down, delay); }

    void KeyboardKeyUp(KeyboardKey key, int delay = 0)
        { AddKeyboardKeyEvent(key, ::Intercept::KeyboardKeyEvent::Up, delay); }
    
    Point MousePosition() const;
    bool MouseMoved(int delta = 0);
    bool LeftMouseButtonPressed()   { return m_intercept.MouseButtonPressed(::Intercept::MouseButton::Left); }
    bool RightMouseButtonPressed()  { return m_intercept.MouseButtonPressed(::Intercept::MouseButton::Right); }
    bool KeyboardKeyPressed(KeyboardKey key);

    void Send();
    void Reset() { m_events.clear(); }
    virtual bool Ready() const { return m_ready.load(); }

private:
    struct MouseMoveEvent { ::Intercept::Point position; };
    struct MouseButtonEvent { ::Intercept::MouseButtonEvent event; };

    struct KeyboardKeyEvent
    {
        int code;
        ::Intercept::KeyboardKeyEvent event;
        bool e0;
        bool e1;
    };

    using Event = std::variant<MouseMoveEvent, MouseButtonEvent, KeyboardKeyEvent>;

    ::Intercept m_intercept;
    std::vector<std::pair<Event, int>> m_events;
    Point m_mouse_position;
    std::atomic_bool m_ready;

    void AddMouseButtonEvent(::Intercept::MouseButtonEvent event, int delay);
    void AddKeyboardKeyEvent(KeyboardKey key, ::Intercept::KeyboardKeyEvent event, int delay);
    void AddEvent(Event event, int delay) { if (Ready()) m_events.push_back({event, delay}); }

    static int KeyScanCode(KeyboardKey key) { return static_cast<int>(key) & ~(SHIFT | E0 | E1); }
};
