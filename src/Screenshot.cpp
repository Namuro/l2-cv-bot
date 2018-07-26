#include "Screenshot.h"

std::optional<cv::Rect> Screenshot::WindowRect(const std::string &window_title)
{
    // convert std::string to std::wstring for Windows API
    auto wide_window_title = Widen(window_title);

    if (!wide_window_title.has_value()) {
        return {};
    }

    auto needed_title = wide_window_title.value().c_str();

    // save all HWNDs
    std::vector<HWND> hwnds;
    ::EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&hwnds));

    // search for exact title match
    HWND found_hwnd = nullptr;
    std::map<HWND, std::wstring> hwnd_titles;
    wchar_t title[256];

    for (auto &hwnd : hwnds) {
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
    if (found_hwnd == nullptr) {
        return {};
    }

    RECT rect = {};

    if (!::GetClientRect(found_hwnd, reinterpret_cast<LPRECT>(&rect))) {
        return {};
    }

    if (::SetLastError(ERROR_SUCCESS);
        ::MapWindowPoints(found_hwnd, nullptr, reinterpret_cast<LPPOINT>(&rect), 2) == 0 &&
            ::GetLastError() != ERROR_SUCCESS
    ) {
        return {};
    }

    cv::Rect found_rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

    if (found_rect.width > 0 && found_rect.height > 0) {
        return found_rect;
    }

    return {};
}

std::optional<std::wstring> Screenshot::Widen(const std::string &string)
{
    if (string.empty()) {
        return std::wstring();
    }

    auto chars_needed = ::MultiByteToWideChar(
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

    auto chars_converted = ::MultiByteToWideChar(
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

BOOL CALLBACK Screenshot::EnumWindowsProc(HWND hwnd, LPARAM lparam)
{
    if (hwnd != nullptr && ::IsWindowVisible(hwnd) && ::IsWindowEnabled(hwnd)) {
        auto hwnds = reinterpret_cast<std::vector<HWND> *>(lparam);
        hwnds->push_back(hwnd);
    }

    return TRUE;
}
