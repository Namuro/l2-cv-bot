#pragma once

#include <vector>
#include <thread>
#include <mutex>

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

class Input
{
public:
    struct Point { int x, y = 0; };

private:
    struct HOOKUnhooker
    {
        using pointer = ::HHOOK;
        void operator()(::HHOOK hook) const { ::UnhookWindowsHookEx(hook); }
    };

    // workaround
    static std::function<void(int)> s_kb_callback;
    static ::HHOOK s_hook;

    int m_width, m_height = 0;
    std::vector<std::pair<::INPUT, int>> m_inputs;
    std::unique_ptr<::HHOOK, HOOKUnhooker> m_hook;
    Point m_mouse_position;
    std::mutex m_mouse_position_mtx;

public:
    Input() :
        m_width(::GetSystemMetrics(SM_CXVIRTUALSCREEN)),
        m_height(::GetSystemMetrics(SM_CYVIRTUALSCREEN)),
        m_hook(::SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardCallback, nullptr, 0), HOOKUnhooker()),
        m_mouse_position(MousePosition()) { s_hook = m_hook.get(); }

    Point MousePosition() const
    {
        ::POINT point;
        ::GetCursorPos(&point);
        return { point.x, point.y };
    }

    bool MouseMoved(int dx = 100, int dy = 100)
    {
        const auto position = MousePosition();
        std::lock_guard guard(m_mouse_position_mtx);
        return std::abs(position.x - m_mouse_position.x) >= dx ||
            std::abs(position.y - m_mouse_position.y) >= dy;
    }

    void MouseMove(int x, int y, int delay = 0);
    void MouseLeftDown(int delay = 0);
    void MouseLeftUp(int delay = 0);
    void MouseRightDown(int delay = 0);
    void MouseRightUp(int delay = 0);
    void Send();
    void Reset() { m_inputs.clear(); }
    void RegisterKeyboardCallback(decltype(s_kb_callback) callback) { s_kb_callback = callback; }

private:
    static ::LRESULT CALLBACK KeyboardCallback(int code, ::WPARAM wparam, ::LPARAM lparam);
};
