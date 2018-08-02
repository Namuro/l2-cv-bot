#include "Window.h"

std::optional<Window> Window::Find(const std::string &window_title)
{
    const auto wide_window_title = WidenString(window_title);

    if (!wide_window_title.has_value()) {
        return {};
    }

    const auto needed_title = wide_window_title.value().c_str();

    std::vector<::HWND> hwnds;

    ::EnumWindows([](::HWND hwnd, ::LPARAM lparam) -> ::BOOL {
        if (hwnd != nullptr && ::IsWindowVisible(hwnd) && ::IsWindowEnabled(hwnd)) {
            const auto hwnds = reinterpret_cast<std::vector<::HWND> *>(lparam);
            hwnds->push_back(hwnd);
        }

        return TRUE;
    }, reinterpret_cast<::LPARAM>(&hwnds));

    ::HWND found_hwnd = nullptr;

    // search for exact title match
    std::map<::HWND, std::wstring> hwnd_titles;

    for (const auto &hwnd : hwnds) {
        wchar_t title[256];

        if (::GetWindowTextW(hwnd, reinterpret_cast<::LPWSTR>(title), sizeof(title) - 1) == 0) {
            continue;
        }

        if (std::wcscmp(title, needed_title) == 0) {
            found_hwnd = hwnd;
            break;
        }

        hwnd_titles[hwnd] = std::wstring(title);
    }

    // search for partial title match
    if (found_hwnd == nullptr) {
        for (const auto &pair : hwnd_titles) {
            const auto hwnd = pair.first;
            const auto title = pair.second.c_str();

            if (std::wcsstr(title, needed_title) != nullptr) {
                found_hwnd = hwnd;
                break;
            }
        }
    }

    const auto rect = HWNDRect(found_hwnd);

    if (!rect.has_value()) {
        return {};
    }

    return Window(found_hwnd, rect.value());
}

std::optional<struct Window::Rect> Window::HWNDRect(const ::HWND hwnd)
{
    if (hwnd == nullptr) {
        return {};
    }

    ::RECT rect = {};

    if (!::GetClientRect(hwnd, reinterpret_cast<::LPRECT>(&rect))) {
        return {};
    }

    ::POINT lt = {rect.left, rect.top};
    ::POINT rb = {rect.right, rect.bottom};

    if (!::ClientToScreen(hwnd, &lt) || !::ClientToScreen(hwnd, &rb)) {
        return {};
    }

    return {{lt.x, lt.y, rb.x - lt.x, rb.y - lt.y}};
}

std::optional<std::wstring> Window::WidenString(const std::string &string)
{
    if (string.empty()) {
        return std::wstring();
    }

    const auto chars_needed = ::MultiByteToWideChar(
        CP_UTF8,
        0,
        reinterpret_cast<::LPCCH>(&string[0]),
        static_cast<int>(string.size()),
        nullptr,
        0
    );

    if (chars_needed == 0) {
        return {};
    }

    std::wstring wstring(chars_needed, '\0');

    const auto chars_converted = ::MultiByteToWideChar(
        CP_UTF8,
        0,
        reinterpret_cast<::LPCCH>(&string[0]),
        static_cast<int>(string.size()),
        &wstring[0],
        chars_needed
    );

    if (chars_converted != chars_needed) {
        return {};
    }

    return wstring;
}
