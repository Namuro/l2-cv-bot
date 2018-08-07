#pragma once

#include "Input.h"

class Hands : public Input
{
public:
    struct Rect { int x, y, width, height; };

    Hands() : Input{}, m_window_rect{} {}

    void SetWindowRect(const Rect &rect)    { m_window_rect = rect; }
    
    void SelectTarget(const Point &point)   { MoveMouseSmoothly(WindowPoint(point)); LeftMouseButtonDoubleClick(); }
    void CancelTarget()                     { PressKeyboardKey(KeyboardKey::Escape); }
    void GoTo(const Point &point)           { MoveMouseSmoothly(WindowPoint(point)); LeftMouseButtonClick(); }
    void PickUp()                           { KeyboardKeyDown(KeyboardKey::F5); Delay(3000); KeyboardKeyUp(KeyboardKey::F5); }
    void NextTarget()                       { PressKeyboardKey(KeyboardKey::F2); }
    void Attack()                           { PressKeyboardKey(KeyboardKey::F1); }

    void ResetUI()
    {
        MoveMouseSmoothly(WindowCenter());
        RightMouseButtonClick();
        PressKeyboardKeyCombination(KeyboardKey::LeftAlt, KeyboardKey::L);
    }

    void LookAround()
    {
        const auto center = WindowCenter();
        MoveMouseSmoothly({center.x + 50, center.y + 50});
        LeftMouseButtonClick();
        Delay(500);
        RightMouseButtonClick();
    }

private:
    Rect m_window_rect;

    Point WindowPoint(const Point &point) const { return {m_window_rect.x + point.x, m_window_rect.y + point.y}; }

    Point WindowCenter() const
        { return {m_window_rect.x + m_window_rect.width / 2, m_window_rect.y + m_window_rect.height / 2}; }
};
