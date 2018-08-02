#include "Capture.h"

Capture::Capture() :
    m_bmi(),
    m_x(::GetSystemMetrics(SM_XVIRTUALSCREEN)),
    m_y(::GetSystemMetrics(SM_YVIRTUALSCREEN)),
    m_width(::GetSystemMetrics(SM_CXVIRTUALSCREEN)),
    m_height(::GetSystemMetrics(SM_CYVIRTUALSCREEN)),
    m_srcdc(::GetDC(nullptr), DCReleaser(nullptr)),
    m_memdc(::CreateCompatibleDC(nullptr), DCDeleter())
{
    m_bmi.bmiHeader.biSize = sizeof(m_bmi.bmiHeader);
    m_bmi.bmiHeader.biWidth = m_width;
    m_bmi.bmiHeader.biHeight = -m_height;
    m_bmi.bmiHeader.biPlanes = 1;
    m_bmi.bmiHeader.biBitCount = 32;
    m_bmi.bmiHeader.biCompression = BI_RGB;

    m_bitmap = { ::CreateDIBSection(
        m_memdc.get(),
        &m_bmi,
        DIB_RGB_COLORS,
        reinterpret_cast<void **>(&m_data),
        nullptr,
        0
    ), BITMAPDeleter() };

    m_object = { ::SelectObject(m_memdc.get(), m_bitmap.get()), GDIOBJDeselector(m_memdc.get()) };
}

std::optional<Capture::Bitmap> Capture::Grab(const Rect &rect)
{
    if (rect.width <= 0 || rect.height <= 0) {
        return {};
    }

    if (!::BitBlt(m_memdc.get(), 0, 0, rect.width, rect.height, m_srcdc.get(), rect.x, rect.y, SRCCOPY | CAPTUREBLT)) {
        return {};
    }

    Bitmap bitmap = {};
    bitmap.data = m_data;
    bitmap.rows = m_height;
    bitmap.cols = m_width;
    bitmap.width = (std::min)(m_width, rect.width);
    bitmap.height = (std::min)(m_height, rect.height);
    bitmap.bits = m_bmi.bmiHeader.biBitCount;
    return bitmap;
}

bool Capture::Clear()
{
    return ::BitBlt(m_memdc.get(), 0, 0, m_width, m_height, nullptr, m_x, m_y, BLACKNESS) == TRUE;
}
