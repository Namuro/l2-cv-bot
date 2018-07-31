#include "Capture.h"

Capture::Capture() :
    m_bmi(),

    // get screen dimensions
    m_x(::GetSystemMetrics(SM_XVIRTUALSCREEN)),
    m_y(::GetSystemMetrics(SM_YVIRTUALSCREEN)),
    m_width(::GetSystemMetrics(SM_CXVIRTUALSCREEN)),
    m_height(::GetSystemMetrics(SM_CYVIRTUALSCREEN)),

    // initialize contexts
    m_srcdc(::GetDC(nullptr), DCReleaser(nullptr)),
    m_memdc(::CreateCompatibleDC(nullptr), DCDeleter())
{
    // setup bitmap info
    m_bmi.bmiHeader.biSize = sizeof(m_bmi.bmiHeader);
    m_bmi.bmiHeader.biWidth = m_width;
    m_bmi.bmiHeader.biHeight = -m_height;
    m_bmi.bmiHeader.biPlanes = 1;
    m_bmi.bmiHeader.biBitCount = 32;
    m_bmi.bmiHeader.biCompression = BI_RGB;

    // initialize & select bitmap
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

std::optional<Capture::Bitmap> Capture::Grab(int x, int y, int width, int height)
{
    if (width <= 0 || height <= 0) {
        return {};
    }

    // copy pixels from source context to memory context
    if (!::BitBlt(m_memdc.get(), 0, 0, width, height, m_srcdc.get(), x, y, SRCCOPY | CAPTUREBLT)) {
        return {};
    }

    Bitmap bitmap = {};
    bitmap.data = m_data;
    bitmap.rows = m_height;
    bitmap.cols = m_width;
    bitmap.width = (std::min)(m_width, width);
    bitmap.height = (std::min)(m_height, height);
    bitmap.bits = m_bmi.bmiHeader.biBitCount;
    return bitmap;
}

bool Capture::Clear()
{
    return ::BitBlt(m_memdc.get(), 0, 0, m_width, m_height, nullptr, m_x, m_y, BLACKNESS) == TRUE;
}
