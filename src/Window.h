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

    bool BringToForeground() const { return ::SetForegroundWindow(m_hwnd); }
    Point ConvertPoint(const Point &point) const { return {m_rect.x + point.x, m_rect.y + point.y}; }
    Point Center() const { return {m_rect.x + m_rect.width / 2, m_rect.y + m_rect.height / 2}; }

private:
    ::HWND m_hwnd;
    struct Rect m_rect;

    Window(::HWND hwnd, const struct Rect &rect) : m_hwnd{hwnd}, m_rect{rect} {}

    static std::optional<struct Rect> HWNDRect(const ::HWND hwnd);
    static std::optional<std::wstring> WidenString(const std::string &string);
};
