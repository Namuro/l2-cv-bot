#include <thread>

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

#include "Intercept.h"

Intercept::Intercept() :
    m_screen_width          {::GetSystemMetrics(SM_CXVIRTUALSCREEN)},
    m_screen_height         {::GetSystemMetrics(SM_CYVIRTUALSCREEN)},
    m_context               {},
    m_keyboard_device       {0},
    m_mouse_device          {0},
    m_pressed_keyboard_keys {},
    m_pressed_mouse_buttons {},
    m_mouse_delta           {},
    m_keyboard_mtx          {},
    m_mouse_mtx             {}
{
    const auto context = ::interception_create_context();

    if (context == nullptr) {
        throw InterceptionDriverNotFoundError();
    }

    m_context = {context, InterceptionContextDestroyer()};

    // find default keyboard device
    for (::InterceptionDevice device = 0; device < INTERCEPTION_MAX_KEYBOARD; ++device) {
        char hardware_id[512]; // not used

        if (::interception_get_hardware_id(context, INTERCEPTION_KEYBOARD(device), hardware_id, sizeof(hardware_id)) > 0) {
            m_keyboard_device = INTERCEPTION_KEYBOARD(device);
            break;
        }
    }

    // find default mouse device
    for (::InterceptionDevice device = 0; device < INTERCEPTION_MAX_MOUSE; ++device) {
        char hardware_id[512]; // not used

        if (::interception_get_hardware_id(context, INTERCEPTION_MOUSE(device), hardware_id, sizeof(hardware_id)) > 0) {
            m_mouse_device = INTERCEPTION_MOUSE(device);
            break;
        }
    }

    // intercept all keyboard and mouse events
    ::interception_set_filter(context, ::interception_is_keyboard, INTERCEPTION_FILTER_KEY_ALL);
    ::interception_set_filter(context, ::interception_is_mouse, INTERCEPTION_FILTER_MOUSE_ALL);

    std::thread([this](::InterceptionContext context) {
        ::InterceptionDevice device = 0;
        ::InterceptionStroke stroke = {};

        while (::interception_receive(context, device = ::interception_wait(context), &stroke, 1) > 0) {
            ::interception_send(context, device, &stroke, 1);

            if (::interception_is_keyboard(device)) {
                std::lock_guard lock(m_keyboard_mtx);
                const auto key_stroke = reinterpret_cast<::InterceptionKeyStroke *>(&stroke);

                if (key_stroke->code < m_pressed_keyboard_keys.size()) {
                    m_pressed_keyboard_keys[key_stroke->code] =
                        key_stroke->state == INTERCEPTION_KEY_DOWN ||
                        key_stroke->state == INTERCEPTION_KEY_E0;
                }
            } else if (::interception_is_mouse(device)) {
                std::lock_guard lock(m_mouse_mtx);
                const auto mouse_stroke = reinterpret_cast<InterceptionMouseStroke *>(&stroke);

                if (mouse_stroke->state != 0) {
                    m_pressed_mouse_buttons[static_cast<size_t>(MouseButton::Left)] =
                        mouse_stroke->state & INTERCEPTION_MOUSE_LEFT_BUTTON_DOWN;
                    m_pressed_mouse_buttons[static_cast<size_t>(MouseButton::Right)] =
                        mouse_stroke->state & INTERCEPTION_MOUSE_RIGHT_BUTTON_DOWN;
                    m_pressed_mouse_buttons[static_cast<size_t>(MouseButton::Middle)] =
                        mouse_stroke->state & INTERCEPTION_MOUSE_MIDDLE_BUTTON_DOWN;
                    m_pressed_mouse_buttons[static_cast<size_t>(MouseButton::Fourth)] =
                        mouse_stroke->state & INTERCEPTION_MOUSE_BUTTON_4_DOWN;
                    m_pressed_mouse_buttons[static_cast<size_t>(MouseButton::Fifth)] =
                        mouse_stroke->state & INTERCEPTION_MOUSE_BUTTON_5_DOWN;
                } else {
                    m_mouse_delta.x += std::abs(mouse_stroke->x);
                    m_mouse_delta.y += std::abs(mouse_stroke->y);
                }
            }
        }
    }, context).detach();
}

void Intercept::SendMouseMoveEvent(const Point &point) const
{
    ::InterceptionMouseStroke stroke = {};
    stroke.flags = INTERCEPTION_MOUSE_MOVE_ABSOLUTE | INTERCEPTION_MOUSE_VIRTUAL_DESKTOP;
    stroke.x = point.x * 0xFFFF / m_screen_width + 1;
    stroke.y = point.y * 0xFFFF / m_screen_height + 1;
    ::interception_send(m_context.get(), m_mouse_device, reinterpret_cast<InterceptionStroke *>(&stroke), 1);
}

void Intercept::SendMouseButtonEvent(MouseButtonEvent event) const
{
    ::InterceptionMouseStroke stroke = {};
    stroke.state = static_cast<unsigned short>(event);
    ::interception_send(m_context.get(), m_mouse_device, reinterpret_cast<InterceptionStroke *>(&stroke), 1);
}

void Intercept::SendKeyboardKeyEvent(int code, KeyboardKeyEvent event, bool e0, bool e1) const
{
    const auto state = static_cast<unsigned short>(event);

    ::InterceptionKeyStroke stroke = {};
    stroke.code = static_cast<unsigned short>(code);
    stroke.state = state;

    if (e0) {
        stroke.state |= INTERCEPTION_KEY_E0;
    }

    if (e1) {
        stroke.state |= INTERCEPTION_KEY_E1;
    }

    ::interception_send(m_context.get(), m_keyboard_device, reinterpret_cast<InterceptionStroke *>(&stroke), 1);
}

bool Intercept::MouseButtonPressed(MouseButton button)
{
    std::lock_guard lock(m_mouse_mtx);
    return m_pressed_mouse_buttons[static_cast<size_t>(button)];
}

bool Intercept::KeyboardKeyPressed(int code)
{
    std::lock_guard lock(m_keyboard_mtx);

    if (code >= m_pressed_keyboard_keys.size()) {
        return false;
    }

    return m_pressed_keyboard_keys[code];
}

Intercept::Point Intercept::MouseDelta()
{
    std::lock_guard lock(m_mouse_mtx);
    return m_mouse_delta;
}
