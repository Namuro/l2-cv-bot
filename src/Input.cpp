#include "Input.h"

::HHOOK Input::s_hook;

Input::Input() :
    m_width(::GetSystemMetrics(SM_CXVIRTUALSCREEN)),
    m_height(::GetSystemMetrics(SM_CYVIRTUALSCREEN)),
    m_hook(::SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardCallback, nullptr, 0), HOOKUnhooker())
{
    s_hook = m_hook.get(); // workaround to pass to static KeyboardCallback

    // listen for Windows messages
    std::thread([]() {
        ::MSG msg;

        while (::GetMessage(&msg, nullptr, 0, 0)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }).detach();
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

void Input::Send()
{
    if (m_inputs.empty()) {
        return;
    }

    // call SendInput in background thread
    std::thread([](const std::vector<std::pair<::INPUT, int>> inputs) { // m_inputs copied
        for (const auto &pair : inputs) {
            ::INPUT input = pair.first;
            const ::DWORD delay = pair.second;

            if (delay > 0) {
                ::Sleep(delay);
            }

            ::SendInput(1, reinterpret_cast<::LPINPUT>(&input), sizeof(::INPUT));
        }
    }, m_inputs).detach();

    Reset();
}

::LRESULT CALLBACK Input::KeyboardCallback(int code, ::WPARAM wparam, ::LPARAM lparam)
{
    // called on main thread so no locks required
    if (code == HC_ACTION) {
        const auto pkb = reinterpret_cast<::PKBDLLHOOKSTRUCT>(lparam);

        switch (wparam) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (pkb->vkCode == VK_ESCAPE) {
                
            }

            break;
        default:
            break;
        }
    }

    return ::CallNextHookEx(s_hook, code, wparam, lparam);
}
