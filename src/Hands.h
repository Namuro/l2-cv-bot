#pragma once

#include "Input.h"

class Hands : public Input
{
public:
    KeyboardKey m_attack_key        = KeyboardKey::F1;
    KeyboardKey m_next_target_key   = KeyboardKey::F2;
    KeyboardKey m_spoil_key         = KeyboardKey::F3;
    KeyboardKey m_sweep_key         = KeyboardKey::F4;
    KeyboardKey m_pick_up_key       = KeyboardKey::F5;
    KeyboardKey m_restore_hp_key    = KeyboardKey::F6;
    KeyboardKey m_restore_mp_key    = KeyboardKey::F7;
    KeyboardKey m_restore_cp_key    = KeyboardKey::F8;

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
    Hands &Attack()         { PressKeyboardKey(m_attack_key); return *this; }
    Hands &NextTarget()     { PressKeyboardKey(m_next_target_key); return *this; }
    Hands &Spoil()          { PressKeyboardKey(m_spoil_key, 200); return *this; }
    Hands &Sweep()          { PressKeyboardKey(m_sweep_key, 200); return *this; }
    Hands &PickUp()         { PressKeyboardKey(m_pick_up_key, 3500); return *this; }
    Hands &RestoreHP()      { PressKeyboardKey(m_restore_hp_key, 200); return *this; }
    Hands &RestoreMP()      { PressKeyboardKey(m_restore_mp_key, 200); return *this; }
    Hands &RestoreCP()      { PressKeyboardKey(m_restore_cp_key, 200); return *this; }

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
