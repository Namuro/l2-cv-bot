#pragma once

#include <vector>
#include <mutex>
#include <atomic>

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

class Input
{
public:
    struct Point { int x, y; };

private:
    int m_width, m_height;
    std::vector<std::pair<::INPUT, int>> m_inputs;
    Point m_mouse_position;
    std::mutex m_mouse_position_mtx;
    std::atomic_bool m_ready;

public:
    Input() :
        m_width(::GetSystemMetrics(SM_CXVIRTUALSCREEN)),
        m_height(::GetSystemMetrics(SM_CYVIRTUALSCREEN)),
        m_mouse_position(MousePosition()),
        m_ready(true) {}

    Point MousePosition() const
    {
        ::POINT point;
        ::GetCursorPos(&point);
        return { point.x, point.y };
    }

    bool MouseMoved(int dx, int dy)
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
    bool Ready() const { return m_ready.load(); }

    bool KeyboardEscapePressed() const { return KeyboardKeyPressed(VK_ESCAPE); }
    bool KeyboardSpacePressed() const { return KeyboardKeyPressed(VK_SPACE); }
    bool KeyboardPrtScnPressed() const { return KeyboardKeyPressed(VK_SNAPSHOT); }
    bool KeyboardF12Pressed() const { return KeyboardKeyPressed(VK_F12); }

private:
    bool KeyboardKeyPressed(int key) const { return ::GetAsyncKeyState(key) & 0x8000; }

    void AddInput(::INPUT input, int delay) {
        if (!Ready()) {
            return;
        }

        m_inputs.push_back({ input, delay });
    }
};
