#include "Capture.h"

Capture::Capture() :
    // get screen dimensions
    m_x(::GetSystemMetrics(SM_XVIRTUALSCREEN)),
    m_y(::GetSystemMetrics(SM_YVIRTUALSCREEN)),
    m_w(::GetSystemMetrics(SM_CXVIRTUALSCREEN)),
    m_h(::GetSystemMetrics(SM_CYVIRTUALSCREEN)),

    // initialize source context
    m_srcdc(srcdc_handle(::GetDC(nullptr), DCReleaser(nullptr)))
{
    // initialize contexts & bitmap
    m_memdc = memdc_handle(::CreateCompatibleDC(m_srcdc.get()), DCDeleter());
    m_bitmap = bitmap_handle(::CreateCompatibleBitmap(m_srcdc.get(), m_w, m_h), BITMAPDeleter());
    m_object = gdiobj_handle(::SelectObject(m_memdc.get(), m_bitmap.get()), GDIOBJDeselector(m_memdc.get()));

    // setup bitmap info
    m_bmi.bmiHeader.biSize = sizeof(m_bmi.bmiHeader);
    m_bmi.bmiHeader.biWidth = m_w;
    m_bmi.bmiHeader.biHeight = -m_h;
    m_bmi.bmiHeader.biPlanes = 1;
    m_bmi.bmiHeader.biBitCount = 24;
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
}

std::optional<cv::Mat> Capture::Grab(cv::Rect rect)
{
    if (rect.width <= 0 || rect.height <= 0) {
        return {};
    }

    if (!::BitBlt(m_memdc.get(), 0, 0, rect.width, rect.height, m_srcdc.get(), rect.x, rect.y, SRCCOPY | CAPTUREBLT)) {
        return {};
    }

    auto lines_copied = ::GetDIBits(
        m_memdc.get(),
        m_bitmap.get(),
        0,
        static_cast<UINT>(m_h),
        reinterpret_cast<LPVOID>(m_data.get()),
        reinterpret_cast<LPBITMAPINFO>(&m_bmi),
        DIB_RGB_COLORS
    );

    if (lines_copied == 0 || lines_copied == ERROR_INVALID_PARAMETER) {
        return {};
    }

    rect.x = 0;
    rect.y = 0;
    return cv::Mat(m_h, m_w, CV_8UC3, m_data.get())(rect);
}

bool Capture::Clear()
{
    return ::BitBlt(m_memdc.get(), 0, 0, m_w, m_h, nullptr, m_x, m_y, BLACKNESS) == TRUE;
}
