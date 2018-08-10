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
    
    Hands &SelectTarget(const Point &point)
        { MoveMouseSmoothly(WindowPoint(point)).Delay(50).LeftMouseButtonClick(); return *this; }

    Hands &ResetUI()
        { PressKeyboardKeyCombination({KeyboardKey::LeftAlt, KeyboardKey::L}); return *this; }

    Hands &CancelTarget()           { PressKeyboardKey(KeyboardKey::Escape); return *this; }
    Hands &GoTo(const Point &point) { MoveMouseSmoothly(WindowPoint(point)).LeftMouseButtonClick(); return *this; }
    Hands &NextTarget()             { PressKeyboardKey(KeyboardKey::F2); return *this; }
    Hands &Attack()                 { PressKeyboardKey(KeyboardKey::F1); return *this; }
    Hands &ResetCamera()            { MoveMouseSmoothly(WindowCenter()).RightMouseButtonClick(); return *this; }

    Hands &LookAround()
    {
        const auto center = WindowCenter();

        MoveMouseSmoothly({center.x + 50, center.y + 50}).
            LeftMouseButtonClick().
            Delay(500).
            RightMouseButtonClick();

        return *this;
    }

    Hands &PickUp()
    {
        for (std::size_t i = 0; i < 50; ++i) {
            PressKeyboardKey(KeyboardKey::F5);
        }

        return *this;
    }

private:
    Rect m_window_rect;

    Point WindowPoint(const Point &point) const { return {m_window_rect.x + point.x, m_window_rect.y + point.y}; }

    Point WindowCenter() const
        { return {m_window_rect.x + m_window_rect.width / 2, m_window_rect.y + m_window_rect.height / 2}; }
};
