#include "Input.h"

decltype(Input::s_key_callback) Input::s_key_callback;
decltype(Input::s_hook) Input::s_hook;

Input::Input() :
    m_width(::GetSystemMetrics(SM_CXVIRTUALSCREEN)),
    m_height(::GetSystemMetrics(SM_CYVIRTUALSCREEN)),
    m_hook(::SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardCallback, nullptr, 0), HOOKUnhooker()),
    m_mouse_position(MousePosition())
{
    s_hook = m_hook.get(); // workaround to pass to static KeyboardCallback

    // listen for keyboard messages
    std::thread([]() {
        ::MSG msg = {};
        ::BOOL ret = FALSE;

        while ((ret = ::GetMessage(&msg, nullptr, 0, 0)) != 0) {
            if (ret == -1) {
                break;
            }

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
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    m_inputs.push_back({ input, delay });
    m_mouse_position = { x, y };
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
    std::thread([this](const std::vector<std::pair<::INPUT, int>> inputs) { // m_inputs -> inputs copy
        for (const auto &pair : inputs) {
            ::INPUT input = pair.first;
            const ::DWORD delay = pair.second;

            if (delay > 0) {
                ::Sleep(delay);
            }

            ::SendInput(1, reinterpret_cast<::LPINPUT>(&input), sizeof(::INPUT));

            // save mouse position
            if (input.type == INPUT_MOUSE && input.mi.dwFlags & MOUSEEVENTF_MOVE) {
                std::lock_guard guard(m_mouse_position_mtx);
                m_mouse_position = { input.mi.dx, input.mi.dy };
            }
        }
    }, m_inputs).detach();

    Reset();
}

::LRESULT CALLBACK Input::KeyboardCallback(int code, ::WPARAM wparam, ::LPARAM lparam)
{
    // called on main thread so no locks required for s_key_callback
    if (code == HC_ACTION && s_key_callback) {
        const auto pkb = reinterpret_cast<::PKBDLLHOOKSTRUCT>(lparam);

        switch (wparam) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            s_key_callback(static_cast<int>(pkb->vkCode));
            break;
        default:
            break;
        }
    }

    return ::CallNextHookEx(s_hook, code, wparam, lparam);
}
