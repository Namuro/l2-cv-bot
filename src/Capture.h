#pragma once

#include <optional>
#include <string>
#include <Windows.h>
#include <opencv2/opencv.hpp>

struct DCDeleter
{
    using pointer = HDC;
    void operator()(HDC hdc) const { ::DeleteDC(hdc); }
};

struct DCReleaser
{
    using pointer = HDC;
    HWND hwnd = nullptr;

    DCReleaser() : hwnd(nullptr) {}
    explicit DCReleaser(HWND hwnd) : hwnd(hwnd) {}
    void operator()(HDC hdc) const { ::ReleaseDC(hwnd, hdc); }
};

struct BITMAPDeleter
{
    using pointer = HBITMAP;
    void operator()(HBITMAP bitmap) const { ::DeleteObject(bitmap); }
};

struct GDIOBJDeselector
{
    using pointer = HGDIOBJ;
    HDC hdc;

    GDIOBJDeselector() : hdc(nullptr) {}
    explicit GDIOBJDeselector(HDC hdc) : hdc(hdc) {}
    void operator()(HGDIOBJ object) const { ::SelectObject(hdc, object); }
};

using srcdc_handle = std::unique_ptr<HDC, DCReleaser>;
using memdc_handle = std::unique_ptr<HDC, DCDeleter>;
using bitmap_handle = std::unique_ptr<HBITMAP, BITMAPDeleter>;
using gdiobj_handle = std::unique_ptr<HGDIOBJ, GDIOBJDeselector>;

class Capture
{
    int m_x, m_y, m_w, m_h = 0;
    BITMAPINFO m_bmi = {};

    srcdc_handle m_srcdc;
    memdc_handle m_memdc;
    gdiobj_handle m_object;
    bitmap_handle m_bitmap;
    std::unique_ptr<uchar[]> m_data;

public:
    Capture();

    std::optional<cv::Mat> Capture::Grab(cv::Rect rect);
    std::optional<cv::Mat> Capture::Grab() { return Grab(cv::Rect(m_x, m_y, m_w, m_h)); }
};
