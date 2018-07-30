#include "Input.h"

Input::Point Input::MousePosition() const
{
    ::POINT point;
    ::GetCursorPos(&point);
    return { point.x, point.y };
}

void Input::MouseMove(int x, int y, int delay)
{
    ::INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dx = x * 0xffff / m_width + 1;
    input.mi.dy = y * 0xffff / m_height + 1;
    input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_VIRTUALDESK;
    m_inputs.push_back({ input, delay });
}

void Input::MouseLeftDown(int delay)
{
    ::INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    m_inputs.push_back({ input, delay });
}

void Input::MouseLeftUp(int delay)
{
    ::INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    m_inputs.push_back({ input, delay });
}

void Input::MouseRightDown(int delay)
{
    ::INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    m_inputs.push_back({ input, delay });
}

void Input::MouseRightUp(int delay)
{
    ::INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    m_inputs.push_back({ input, delay });
}

void Input::KeyboardKeyDown(char key, int delay)
{

}

void Input::KeyboardKeyUp(char key, int delay)
{

}

void Input::Send(bool with_delays)
{
    if (m_inputs.empty()) {
        return;
    }

    if (!with_delays) {
        ::SendInput(static_cast<::UINT>(m_inputs.size()), reinterpret_cast<::LPINPUT>(m_inputs.data()), sizeof(::INPUT));
        m_inputs.clear();
        return;
    }

    // call SendInput in background thread
    std::thread([](const std::vector<std::pair<::INPUT, int>> inputs) {
        for (const auto &pair : inputs) {
            ::INPUT input = pair.first;
            const ::DWORD delay = pair.second;

            if (delay > 0) {
                ::Sleep(delay);
            }

            ::SendInput(1, reinterpret_cast<::LPINPUT>(&input), sizeof(::INPUT));
        }
    }, m_inputs).detach();

    m_inputs.clear();
}
