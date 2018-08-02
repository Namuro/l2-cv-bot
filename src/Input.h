#pragma once

#include <vector>
#include <mutex>
#include <atomic>

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

class Input
{
public:
    enum Key
    {
        KEY_ESCAPE = -1,
        KEY_SPACE = -2,
        KEY_PRTSCN = -3,
        KEY_F12 = -4,
    };

    struct Point { int x, y; };

private:
    int m_width, m_height;
    std::vector<std::pair<::INPUT, int>> m_inputs;
    Point m_mouse_position;
    std::mutex m_mouse_position_mtx;
    std::atomic_bool m_ready;

public:
    Input() :
        m_width{::GetSystemMetrics(SM_CXVIRTUALSCREEN)},
        m_height{::GetSystemMetrics(SM_CYVIRTUALSCREEN)},
        m_mouse_position{MousePosition()},
        m_ready{true} {}

    Point MousePosition() const
    {
        ::POINT point;
        ::GetCursorPos(&point);
        return {point.x, point.y};
    }

    bool MouseMoved(int delta = 0)
    {
        const auto position = MousePosition();
        std::lock_guard guard(m_mouse_position_mtx);
        return std::abs(position.x - m_mouse_position.x) > delta ||
            std::abs(position.y - m_mouse_position.y) > delta;
    }

    void MouseMove(const Point &point, int delay = 0);
    void MouseMoveSmoothly(const Point &point, int step = 30, int delay = 10);
    void MouseLeftDown(int delay = 0);
    void MouseLeftUp(int delay = 0);
    void MouseRightDown(int delay = 0);
    void MouseRightUp(int delay = 0);

    void KeyboardKeyDown(Key key, int delay = 0);
    void KeyboardKeyUp(Key key, int delay = 0);
    bool KeyboardKeyPressed(Key key) const { return ::GetAsyncKeyState(KeyToVK(key)) & 0x8000; }

    void Send();
    void Reset() { m_inputs.clear(); }
    bool Ready() const { return m_ready.load(); }

private:
    // mouse coordinates conversions for Windows API
    int XDX(int x) { return x * 0xffff / m_width + 1; }
    int YDY(int y) { return y * 0xffff / m_height + 1; }
    int DXX(int dx) { return (dx * m_width) / 0xffff; }
    int DYY(int dy) { return (dy * m_height) / 0xffff; }

    void AddInput(::INPUT input, int delay) { if (Ready()) m_inputs.push_back({input, delay}); }

    static int KeyToVK(Key key)
    {
        switch (key) {
        case KEY_ESCAPE:    return VK_ESCAPE;
        case KEY_SPACE:     return VK_SPACE;
        case KEY_PRTSCN:    return VK_SNAPSHOT;
        case KEY_F12:       return VK_F12;
        }

        return ::VkKeyScan(static_cast<char>(key)) & 0xff;
    }
};
