#include "Intercept.h"

#include <thread>

std::optional<std::unique_ptr<Intercept>> Intercept::Create()
{
    const auto context = ::interception_create_context();

    if (context == nullptr) {
        return {};
    }

    // allocate on heap to move mutex container into std::optional
    return std::make_unique<Intercept>(context);
}

Intercept::Intercept(::InterceptionContext context) :
    m_screen_width      {::GetSystemMetrics(SM_CXVIRTUALSCREEN)},
    m_screen_height     {::GetSystemMetrics(SM_CYVIRTUALSCREEN)},
    m_context           {context, InterceptionContextDestroyer()},
    m_keyboard_device   {0},
    m_mouse_device      {0},
    m_keyboard_keys     {},
    m_mouse_buttons     {},
    m_keyboard_mtx      {},
    m_mouse_mtx         {}
{
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

                if (key_stroke->code < m_keyboard_keys.size()) {
                    m_keyboard_keys[key_stroke->code] =
                        key_stroke->state == INTERCEPTION_KEY_DOWN ||
                        key_stroke->state == INTERCEPTION_KEY_E0;
                }
            } else if (::interception_is_mouse(device)) {
                std::lock_guard lock(m_mouse_mtx);
                const auto mouse_stroke = reinterpret_cast<InterceptionMouseStroke *>(&stroke);

                if (mouse_stroke->state != 0) {
                    m_mouse_buttons[static_cast<size_t>(MouseButton::Left)] =
                        mouse_stroke->state & INTERCEPTION_MOUSE_LEFT_BUTTON_DOWN;
                    m_mouse_buttons[static_cast<size_t>(MouseButton::Right)] =
                        mouse_stroke->state & INTERCEPTION_MOUSE_RIGHT_BUTTON_DOWN;
                    m_mouse_buttons[static_cast<size_t>(MouseButton::Middle)] =
                        mouse_stroke->state & INTERCEPTION_MOUSE_MIDDLE_BUTTON_DOWN;
                    m_mouse_buttons[static_cast<size_t>(MouseButton::Fourth)] =
                        mouse_stroke->state & INTERCEPTION_MOUSE_BUTTON_4_DOWN;
                    m_mouse_buttons[static_cast<size_t>(MouseButton::Fifth)] =
                        mouse_stroke->state & INTERCEPTION_MOUSE_BUTTON_5_DOWN;
                }
            }
        }
    }, context).detach();
}

void Intercept::SendMouseMoveEvent(const Point &position) const
{
    ::InterceptionMouseStroke stroke = {};
    stroke.flags = INTERCEPTION_MOUSE_MOVE_ABSOLUTE | INTERCEPTION_MOUSE_VIRTUAL_DESKTOP;
    stroke.x = position.x * 0xFFFF / m_screen_width + 1;
    stroke.y = position.y * 0xFFFF / m_screen_height + 1;
    ::interception_send(m_context.get(), m_mouse_device, reinterpret_cast<InterceptionStroke *>(&stroke), 1);
}

void Intercept::SendMouseButtonEvent(MouseButtonEvent event) const
{
    ::InterceptionMouseStroke stroke = {};
    stroke.state = static_cast<unsigned short>(event);
    ::interception_send(m_context.get(), m_mouse_device, reinterpret_cast<InterceptionStroke *>(&stroke), 1);
}

void Intercept::SendKeyboardKeyEvent(KeyboardKey key, KeyboardKeyEvent event) const
{
    const auto code = static_cast<int>(key);
    const auto state = static_cast<unsigned short>(event);

    auto stroke_idx = 0;
    ::InterceptionKeyStroke strokes[2] = {};

    if (code & SHIFT) {
        strokes[stroke_idx].code = static_cast<unsigned short>(KeyScanCode(KeyboardKey::LeftShift));
        strokes[stroke_idx].state = state;
        ++stroke_idx;
    }

    strokes[stroke_idx].code = static_cast<unsigned short>(KeyScanCode(key));
    strokes[stroke_idx].state = state;

    if (code & E0) {
        strokes[stroke_idx].state |= INTERCEPTION_KEY_E0;
    }

    if (code & E1) {
        strokes[stroke_idx].state |= INTERCEPTION_KEY_E1;
    }

    ::interception_send(m_context.get(), m_keyboard_device, reinterpret_cast<InterceptionStroke *>(&strokes), stroke_idx + 1);
}

bool Intercept::MouseButtonPressed(MouseButton button)
{
    std::lock_guard lock(m_mouse_mtx);
    return m_mouse_buttons[static_cast<size_t>(button)];
}

bool Intercept::KeyboardKeyPressed(KeyboardKey key)
{
    std::lock_guard lock(m_keyboard_mtx);
    const auto code = static_cast<size_t>(key);
    const auto scancode = KeyScanCode(key);

    if (scancode >= m_keyboard_keys.size()) {
        return false;
    }

    if (code & SHIFT) {
        const auto lshift = static_cast<size_t>(KeyboardKey::LeftShift);
        const auto rshift = static_cast<size_t>(KeyboardKey::RightShift);

        return m_keyboard_keys[scancode] && (
            m_keyboard_keys[lshift] ||
            m_keyboard_keys[rshift]
        );
    } else {
        return m_keyboard_keys[scancode];
    }
}
