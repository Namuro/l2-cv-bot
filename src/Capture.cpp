#include "Capture.h"

#include <algorithm>

Capture::Capture() :
    m_rect{
        ::GetSystemMetrics(SM_XVIRTUALSCREEN),
        ::GetSystemMetrics(SM_YVIRTUALSCREEN),
        ::GetSystemMetrics(SM_CXVIRTUALSCREEN),
        ::GetSystemMetrics(SM_CYVIRTUALSCREEN)
    },

    m_bmi   {},
    m_srcdc {::GetDC(nullptr), DCReleaser{nullptr}},
    m_memdc {::CreateCompatibleDC(nullptr), DCDeleter{}}
{
    m_bmi.bmiHeader.biSize = sizeof(m_bmi.bmiHeader);
    m_bmi.bmiHeader.biWidth = m_rect.width;
    m_bmi.bmiHeader.biHeight = -m_rect.height;
    m_bmi.bmiHeader.biPlanes = 1;
    m_bmi.bmiHeader.biBitCount = 32;
    m_bmi.bmiHeader.biCompression = BI_RGB;

    m_bitmap = {::CreateDIBSection(
        m_memdc.get(),
        &m_bmi,
        DIB_RGB_COLORS,
        reinterpret_cast<void **>(&m_data),
        nullptr,
        0
    ), BITMAPDeleter{}};

    m_object = {::SelectObject(m_memdc.get(), m_bitmap.get()), GDIOBJDeselector{m_memdc.get()}};
}

std::optional<Capture::Bitmap> Capture::Grab(const struct Rect &rect)
{
    if (rect.width <= 0 || rect.height <= 0) {
        return {};
    }

    if (!::BitBlt(m_memdc.get(), 0, 0, rect.width, rect.height, m_srcdc.get(), rect.x, rect.y, SRCCOPY | CAPTUREBLT)) {
        return {};
    }

    Bitmap bitmap = {};
    bitmap.data = m_data;
    bitmap.rows = m_rect.height;
    bitmap.cols = m_rect.width;
    bitmap.width = (std::min)(m_rect.width, rect.width);
    bitmap.height = (std::min)(m_rect.height, rect.height);
    bitmap.bits = m_bmi.bmiHeader.biBitCount;
    return bitmap;
}
