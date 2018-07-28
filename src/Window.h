#pragma once
#define WIN32_MEAN_AND_LEAN

#include <optional>
#include <string>
#include <Windows.h>
#include <opencv2/opencv.hpp>

class Window
{
public:
    static std::optional<cv::Rect> Rect(const std::string &window_title);

private:
    static std::optional<cv::Rect> HWNDRect(const HWND hwnd);
    static std::optional<std::wstring> WidenString(const std::string &string);
};
