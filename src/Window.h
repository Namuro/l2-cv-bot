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
    struct Rect { int x, y, width, height = 0; };

    static std::optional<Rect> FindRect(const std::string &window_title);

private:
    static std::optional<Rect> GetHWNDRect(const HWND hwnd);
    static std::optional<std::wstring> WidenString(const std::string &string);
};
