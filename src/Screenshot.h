#pragma once

#include <optional>
#include <string>
#include <Windows.h>
#include <opencv2/opencv.hpp>

class Screenshot
{
public:
    std::optional<cv::Mat> TakeWindow(const std::string &window_title);
    std::optional<cv::Rect> WindowRect(const std::string &window_title);

private:
    std::optional<const wchar_t *> Screenshot::Widen(const std::string &string);
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lparam);
};
