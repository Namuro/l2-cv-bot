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

    void ResetUI()
        { PressKeyboardKeyCombination({KeyboardKey::LeftAlt, KeyboardKey::L}); }

    void MoveMouseTo(const Point &point)
        { MoveMouseSmoothly(WindowPoint(point)); }

    void GoTo(const Point &point)
        { MoveMouseTo(point); Delay(200); LeftMouseButtonClick(); }

    void SelectTarget() { LeftMouseButtonClick(); }
    void CancelTarget() { PressKeyboardKey(KeyboardKey::Escape); }
    void ResetCamera()  { MoveMouseSmoothly(WindowCenter()); RightMouseButtonClick(); }
    void Attack()       { PressKeyboardKey(m_attack_key); }
    void NextTarget()   { PressKeyboardKey(m_next_target_key); }
    void Spoil()        { PressKeyboardKey(m_spoil_key); }
    void Sweep()        { PressKeyboardKey(m_sweep_key); }
    void PickUp()       { PressKeyboardKey(m_pick_up_key, 3500); }
    void RestoreHP()    { PressKeyboardKey(m_restore_hp_key); }
    void RestoreMP()    { PressKeyboardKey(m_restore_mp_key); }
    void RestoreCP()    { PressKeyboardKey(m_restore_cp_key); }

    void LookAround()
    {
        const auto center = WindowCenter();
        MoveMouseSmoothly({center.x + 40, center.y + 40});
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
