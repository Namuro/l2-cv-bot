#pragma once

#include <opencv2/opencv.hpp>

#include "Capture.h"

inline cv::Mat BitmapToImage(Capture::Bitmap bitmap)
{
    return cv::Mat(
        bitmap.rows,
        bitmap.cols,
        CV_MAKETYPE(CV_8U, bitmap.bits / 8),
        bitmap.data
    )(cv::Rect(0, 0, bitmap.width, bitmap.height));
}
