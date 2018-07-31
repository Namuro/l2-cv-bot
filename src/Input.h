#pragma once

#include <vector>
#include <thread>

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

struct HOOKUnhooker
{
    using pointer = ::HHOOK;
    void operator()(::HHOOK hook) const { ::UnhookWindowsHookEx(hook); }
};

class Input
{
    int m_width, m_height = 0;
    std::vector<std::pair<::INPUT, int>> m_inputs;
    std::unique_ptr<::HHOOK, HOOKUnhooker> m_hook;
    static std::function<void(int)> s_key_callback; // workaround
    static ::HHOOK s_hook; // workaround

public:
    struct Point { int x, y = 0; };

    Input();

    Point GetMousePos() const
    {
        ::POINT point;
        ::GetCursorPos(&point);
        return { point.x, point.y };
    }

    void MouseMove(int x, int y, int delay = 0);
    void MouseLeftDown(int delay = 0);
    void MouseLeftUp(int delay = 0);
    void MouseRightDown(int delay = 0);
    void MouseRightUp(int delay = 0);
    void KeyboardKeyDown(char key, int delay = 0);
    void KeyboardKeyUp(char key, int delay = 0);
    void Send();
    void Reset() { m_inputs.clear(); }
    void RegisterKeyCallback(decltype(s_key_callback) callback) { s_key_callback = callback; }

private:
    static ::LRESULT CALLBACK KeyboardCallback(int code, ::WPARAM wparam, ::LPARAM lparam);
};
