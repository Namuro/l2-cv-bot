#pragma once

#include <optional>
#include <string>

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

class Window
{
public:
    struct Rect { int x, y, width, height; };
    struct Point { int x, y; };

    static std::optional<Window> Find(const std::string &window_title);

    const Rect &Rect() const { return m_rect; }

    void BringToForeground() const { ::SetForegroundWindow(m_hwnd); }

private:
    const ::HWND m_hwnd;
    const struct Rect m_rect;

    Window(::HWND hwnd, const struct Rect &rect) :
        m_hwnd{hwnd},
        m_rect{rect}
    {}

    static std::optional<struct Rect> HWNDRect(const ::HWND hwnd);
    static std::optional<std::wstring> WidenString(const std::string &string);
};
