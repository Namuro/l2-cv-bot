#include "Screenshot.h"

std::optional<cv::Rect> Screenshot::WindowRect(const std::string &window_title)
{
    // convert std::string to wchar_t* for Windows API
    auto needed_title = Widen(window_title);

    if (!needed_title.has_value()) {
        return {};
    }

    // store all HWNDs
    std::vector<HWND> hwnds;
    ::EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&hwnds));

    // search for exact title match
    HWND found_hwnd = nullptr;
    std::map<HWND, std::wstring> hwnd_titles;
    wchar_t title[256];

    for (auto &hwnd : hwnds) {
        if (::GetWindowTextW(hwnd, reinterpret_cast<LPWSTR>(title), sizeof(title) - 1) > 0) {
            continue;
        }

        if (std::wcscmp(title, needed_title.value()) == 0) {
            found_hwnd = hwnd;
            break;
        }

        std::wstring wstring(title);

        if (!wstring.empty()) {
            hwnd_titles[hwnd] = wstring;
        }
    }

    // search for partial title match
    if (found_hwnd == nullptr) {
        for (auto &pair : hwnd_titles) {
            auto hwnd = pair.first;
            auto title = pair.second.c_str();

            if (std::wcsstr(title, needed_title.value()) != nullptr) {
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

    if (!::GetWindowRect(found_hwnd, &rect)) {
        return {};
    }

    cv::Rect found_rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);

    if (found_rect.width > 0 && found_rect.height > 0 && found_rect.x >= 0 && found_rect.y >= 0) {
        return found_rect;
    }

    return {};
}

std::optional<const wchar_t *> Screenshot::Widen(const std::string &string)
{
    if (string.empty()) {
        return L"";
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

    if (chars_converted == 0) {
        return {};
    }

    return wstring.c_str();
}

BOOL CALLBACK Screenshot::EnumWindowsProc(HWND hwnd, LPARAM lparam)
{
    if (hwnd != nullptr && ::IsWindowVisible(hwnd) && ::IsWindowEnabled(hwnd)) {
        auto hwnds = reinterpret_cast<std::vector<HWND> *>(lparam);
        hwnds->push_back(hwnd);
    }

    return TRUE;
}
