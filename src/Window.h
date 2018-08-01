#pragma once

#include <optional>
#include <string>
#include <vector>
#include <map>

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

class Window
{
public:
    struct Rect { int x, y, width, height; };

private:
    ::HWND m_hwnd;
    Rect m_rect;

public:
    Window(::HWND hwnd, const Rect &rect) : m_hwnd(hwnd), m_rect(rect) {}

    const decltype(m_rect) &Rect() const { return m_rect; }
    bool BringToForeground() const { return SetForegroundWindow(m_hwnd); }

    static std::optional<Window> Find(const std::string &window_title);

private:
    static std::optional<struct Rect> HWNDRect(const HWND hwnd);
    static std::optional<std::wstring> WidenString(const std::string &string);
};
