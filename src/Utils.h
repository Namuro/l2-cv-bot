#pragma once

#include <optional>

#include <opencv2/opencv.hpp>

#include "Capture.h"

inline std::optional<cv::Mat> BitmapToImage(Capture::Bitmap bitmap)
{
    // check bitmap
    if (bitmap.data == nullptr ||
        bitmap.rows <= 0 ||
        bitmap.cols <= 0 ||
        bitmap.width <= 0 ||
        bitmap.height <= 0
    ) {
        return {};
    }

    return cv::Mat(
        bitmap.rows,
        bitmap.cols,
        CV_8UC(bitmap.bits / 8),
        bitmap.data
    )(cv::Rect(0, 0, bitmap.width, bitmap.height));
}
