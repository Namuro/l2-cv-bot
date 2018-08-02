#include "Input.h"

void Input::MouseMove(const Point &point, int delay)
{
    ::INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dx = XDX(point.x);
    input.mi.dy = YDY(point.y);
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    AddInput(input, delay);
}

void Input::MouseLeftDown(int delay)
{
    ::INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    AddInput(input, delay);
}

void Input::MouseLeftUp(int delay)
{
    ::INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    AddInput(input, delay);
}

void Input::MouseRightDown(int delay)
{
    ::INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    AddInput(input, delay);
}

void Input::MouseRightUp(int delay)
{
    ::INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    AddInput(input, delay);
}

void Input::Send()
{
    if (!Ready() || m_inputs.empty()) {
        return;
    }

    m_ready = false;

    std::thread([this](const decltype(m_inputs) inputs) {
        for (const auto &pair : inputs) {
            ::INPUT input = pair.first;
            const ::DWORD delay = pair.second;

            if (delay > 0) {
                ::Sleep(delay);
            }

            ::SendInput(1, reinterpret_cast<::LPINPUT>(&input), sizeof(::INPUT));

            if (input.type == INPUT_MOUSE && input.mi.dwFlags & MOUSEEVENTF_MOVE) {
                std::lock_guard guard(m_mouse_position_mtx);
                m_mouse_position = {DXX(input.mi.dx), DYY(input.mi.dy)};
            }
        }

        m_ready = true;
    }, m_inputs).detach();

    Reset();
}
