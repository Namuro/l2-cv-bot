#pragma once

#include <vector>
#include <thread>

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

class Input
{
    int m_width, m_height = 0;
    std::vector<std::pair<::INPUT, int>> m_inputs;

public:
    struct Point { int x, y = 0; };

    Input() :
        m_width(::GetSystemMetrics(SM_CXVIRTUALSCREEN)),
        m_height(::GetSystemMetrics(SM_CYVIRTUALSCREEN)) {}

    Point MousePosition() const;
    void MouseMove(int x, int y, int delay = 0);
    void MouseLeftDown(int delay = 0);
    void MouseLeftUp(int delay = 0);
    void MouseRightDown(int delay = 0);
    void MouseRightUp(int delay = 0);
    void KeyboardKeyDown(char key, int delay = 0);
    void KeyboardKeyUp(char key, int delay = 0);
    void Send(bool with_delays = false);
};
