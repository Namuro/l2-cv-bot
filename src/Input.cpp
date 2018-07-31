#include "Input.h"

decltype(Input::s_kb_callback) Input::s_kb_callback;
decltype(Input::s_hook) Input::s_hook;

void Input::MouseMove(int x, int y, int delay)
{
    ::INPUT input = {};
    input.type = INPUT_MOUSE;
    input.mi.dx = x * 0xffff / m_width + 1;
    input.mi.dy = y * 0xffff / m_height + 1;
    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
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

void Input::Send()
{
    if (m_inputs.empty()) {
        return;
    }

    // call SendInput in background thread
    std::thread([this](const decltype(m_inputs) inputs) { // m_inputs -> inputs copy
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
                m_mouse_position = MousePosition();
            }
        }
    }, m_inputs).detach();

    Reset();
}

::LRESULT CALLBACK Input::KeyboardCallback(int code, ::WPARAM wparam, ::LPARAM lparam)
{
    // called on main thread so no locks required for s_key_callback
    if (code == HC_ACTION && s_kb_callback) {
        const auto pkb = reinterpret_cast<::PKBDLLHOOKSTRUCT>(lparam);

        switch (wparam) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            s_kb_callback(static_cast<int>(pkb->vkCode));
            break;
        default:
            break;
        }
    }

    return ::CallNextHookEx(s_hook, code, wparam, lparam);
}

//std::thread([]() {
//    ::MSG msg = {};
//    ::BOOL ret = FALSE;

//    while ((ret = ::GetMessage(&msg, nullptr, 0, 0)) != 0) {
//        if (ret == -1) {
//            break;
//        }

//        ::TranslateMessage(&msg);
//        ::DispatchMessage(&msg);
//    }
//}).detach();
