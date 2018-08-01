#pragma once

#include <optional>
#include <memory>
#include <algorithm>

#define WIN32_MEAN_AND_LEAN
#include <Windows.h>

class Capture
{
    struct DCDeleter
    {
        using pointer = ::HDC;
        void operator()(::HDC hdc) const { ::DeleteDC(hdc); }
    };

    struct DCReleaser
    {
        using pointer = ::HDC;
        ::HWND hwnd;

        DCReleaser() : hwnd(nullptr) {}
        explicit DCReleaser(::HWND hwnd) : hwnd(hwnd) {}
        void operator()(::HDC hdc) const { ::ReleaseDC(hwnd, hdc); }
    };

    struct BITMAPDeleter
    {
        using pointer = ::HBITMAP;
        void operator()(::HBITMAP bitmap) const { ::DeleteObject(bitmap); }
    };

    struct GDIOBJDeselector
    {
        using pointer = ::HGDIOBJ;
        ::HDC hdc;

        GDIOBJDeselector() : hdc(nullptr) {}
        explicit GDIOBJDeselector(::HDC hdc) : hdc(hdc) {}
        void operator()(::HGDIOBJ object) const { ::SelectObject(hdc, object); }
    };

    int m_x, m_y, m_width, m_height;
    ::BITMAPINFO m_bmi;

    std::unique_ptr<::HDC, DCReleaser> m_srcdc;
    std::unique_ptr<::HDC, DCDeleter> m_memdc;
    std::unique_ptr<::HGDIOBJ, GDIOBJDeselector> m_object;
    std::unique_ptr<::HBITMAP, BITMAPDeleter> m_bitmap;
    unsigned char *m_data;

public:
    struct Rect { int x, y, width, height; };

    struct Bitmap {
        unsigned char *data;
        int rows;
        int cols;
        int width;
        int height;
        int bits;
    };

    Capture();

    std::optional<Bitmap> Grab(const Rect &rect);
    std::optional<Bitmap> Grab() { return Grab(Rect{ m_x, m_y, m_width, m_height }); }
    bool Clear();
};
