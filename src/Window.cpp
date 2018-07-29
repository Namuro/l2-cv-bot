#include "Window.h"

std::optional<cv::Rect> Window::Rect(const std::string &window_title)
{
    // convert string to wstring for Windows API
    const auto wide_window_title = WidenString(window_title);

    if (!wide_window_title.has_value()) {
        return {};
    }

    const auto needed_title = wide_window_title.value().c_str();

    // collect all HWNDs
    std::vector<HWND> hwnds;

    ::EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL {
        if (hwnd != nullptr && ::IsWindowVisible(hwnd) && ::IsWindowEnabled(hwnd)) {
            auto hwnds = reinterpret_cast<std::vector<HWND> *>(lparam);
            hwnds->push_back(hwnd);
        }

        return TRUE;
    }, reinterpret_cast<LPARAM>(&hwnds));

    HWND found_hwnd = nullptr;

    // search for exact title match
    std::map<HWND, std::wstring> hwnd_titles;

    for (auto &hwnd : hwnds) {
        wchar_t title [256];

        if (::GetWindowTextW(hwnd, reinterpret_cast<LPWSTR>(title), sizeof(title) - 1) == 0) {
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
        for (auto &pair : hwnd_titles) {
            auto hwnd = pair.first;
            auto title = pair.second.c_str();

            if (std::wcsstr(title, needed_title) != nullptr) {
                found_hwnd = hwnd;
                break;
            }
        }
    }

    // get found window's rect
    return HWNDRect(found_hwnd);
}

std::optional<cv::Rect> Window::HWNDRect(const HWND hwnd)
{
    if (hwnd == nullptr) {
        return {};
    }

    RECT rect = {};

    if (!::GetClientRect(hwnd, reinterpret_cast<LPRECT>(&rect))) {
        return {};
    }

    if (::SetLastError(ERROR_SUCCESS);
        ::MapWindowPoints(hwnd, nullptr, reinterpret_cast<LPPOINT>(&rect), 2) == 0 &&
        ::GetLastError() != ERROR_SUCCESS
    ) {
        return {};
    }

    return cv::Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

std::optional<std::wstring> Window::WidenString(const std::string &string)
{
    if (string.empty()) {
        return std::wstring();
    }

    const auto chars_needed = ::MultiByteToWideChar(
        CP_UTF8,
        0,
        reinterpret_cast<LPCCH>(&string[0]),
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
        reinterpret_cast<LPCCH>(&string[0]),
        static_cast<int>(string.size()),
        &wstring[0],
        chars_needed
    );

    if (chars_converted != chars_needed) {
        return {};
    }

    return wstring;
}
