#pragma once

#include "Input.h"

class Hands : public Input
{
public:
    struct Rect { int x, y, width, height; };

    Hands() :
        Input           {},
        m_window_rect   {}
    {}

    void SetWindowRect(const Rect &rect) { m_window_rect = rect; }

    Hands &ResetUI()
        { PressKeyboardKeyCombination({KeyboardKey::LeftAlt, KeyboardKey::L}); return *this; }

    Hands &MoveMouseToTarget(const Point &point)
        { MoveMouseSmoothly(WindowPoint(point)); return *this; }

    Hands &GoTo(const Point &point)
        { MoveMouseSmoothly(WindowPoint(point)).Delay(200).LeftMouseButtonClick(); return *this; }

    Hands &SelectTarget()   { LeftMouseButtonClick(); return *this; }
    Hands &CancelTarget()   { PressKeyboardKey(KeyboardKey::Escape); return *this; }
    Hands &ResetCamera()    { MoveMouseSmoothly(WindowCenter()).RightMouseButtonClick(); return *this; }
    Hands &Attack()         { PressKeyboardKey(KeyboardKey::F1); return *this; }
    Hands &NextTarget()     { PressKeyboardKey(KeyboardKey::F2); return *this; }
    Hands &Spoil()          { PressKeyboardKey(KeyboardKey::F3, 5); return *this; }
    Hands &Sweep()          { PressKeyboardKey(KeyboardKey::F4, 5); return *this; }
    Hands &PickUp()         { PressKeyboardKey(KeyboardKey::F5, 70); return *this; }
    Hands &RestoreHP()      { PressKeyboardKey(KeyboardKey::F6, 5); return *this; }
    Hands &RestoreMP()      { PressKeyboardKey(KeyboardKey::F7, 5); return *this; }
    Hands &RestoreCP()      { PressKeyboardKey(KeyboardKey::F8, 5); return *this; }

    Hands &LookAround()
    {
        const auto center = WindowCenter();

        MoveMouseSmoothly({center.x + 40, center.y + 40}).
            LeftMouseButtonClick().
            Delay(500).
            RightMouseButtonClick();

        return *this;
    }

private:
    Rect m_window_rect;

    Point WindowPoint(const Point &point) const { return {m_window_rect.x + point.x, m_window_rect.y + point.y}; }

    Point WindowCenter() const
        { return {m_window_rect.x + m_window_rect.width / 2, m_window_rect.y + m_window_rect.height / 2}; }
};
