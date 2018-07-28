#include "Capture.h"

Capture::Capture() :
    // get screen dimensions
    m_x(::GetSystemMetrics(SM_XVIRTUALSCREEN)),
    m_y(::GetSystemMetrics(SM_YVIRTUALSCREEN)),
    m_w(::GetSystemMetrics(SM_CXVIRTUALSCREEN)),
    m_h(::GetSystemMetrics(SM_CYVIRTUALSCREEN)),

    // initialize contexts
    m_srcdc(srcdc_handle(::GetDC(nullptr), DCReleaser(nullptr))),
    m_memdc(memdc_handle(::CreateCompatibleDC(nullptr), DCDeleter()))
{
    // setup bitmap info
    m_bmi.bmiHeader.biSize = sizeof(m_bmi.bmiHeader);
    m_bmi.bmiHeader.biWidth = m_w;
    m_bmi.bmiHeader.biHeight = -m_h;
    m_bmi.bmiHeader.biPlanes = 1;
    m_bmi.bmiHeader.biBitCount = GetDeviceCaps(m_srcdc.get(), BITSPIXEL);
    m_bmi.bmiHeader.biCompression = BI_RGB;
    m_bmi.bmiHeader.biClrUsed = 0;
    m_bmi.bmiHeader.biClrImportant = 0;

    // calculate data size
    const auto colors = m_bmi.bmiHeader.biBitCount / 8;
    const auto alignment = 4;
    const auto width = static_cast<int>(std::ceil(m_w * colors / static_cast<double>(alignment))) * alignment;
    m_bmi.bmiHeader.biSizeImage = width * m_h;

    // allocate data for cv::Mat image
    m_data = std::unique_ptr<uchar[]>(new uchar[m_bmi.bmiHeader.biSizeImage]());

    // initialize & select bitmap
    m_bitmap = bitmap_handle(::CreateDIBSection(
        m_memdc.get(),
        &m_bmi,
        DIB_RGB_COLORS,
        reinterpret_cast<void **>(&m_data),
        nullptr,
        0
    ), BITMAPDeleter());

    m_object = gdiobj_handle(::SelectObject(m_memdc.get(), m_bitmap.get()), GDIOBJDeselector(m_memdc.get()));
}

std::optional<cv::Mat> Capture::Grab(cv::Rect rect)
{
    if (rect.width <= 0 || rect.height <= 0) {
        return {};
    }

    if (!::BitBlt(m_memdc.get(), 0, 0, rect.width, rect.height, m_srcdc.get(), rect.x, rect.y, SRCCOPY | CAPTUREBLT)) {
        return {};
    }

    rect.x = 0;
    rect.y = 0;
    return cv::Mat(m_h, m_w, CV_8UC4, m_data.get())(rect);
}

bool Capture::Clear()
{
    return ::BitBlt(m_memdc.get(), 0, 0, m_w, m_h, nullptr, m_x, m_y, BLACKNESS) == TRUE;
}
