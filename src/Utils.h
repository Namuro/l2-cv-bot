#pragma once

#include <optional>

#include <opencv2/opencv.hpp>

#include "Capture.h"

inline std::optional<cv::Mat> BitmapToImage(const Capture::Bitmap &bitmap)
{
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
    )({0, 0, bitmap.width, bitmap.height});
}

inline cv::Scalar VectorToScalar(const std::vector<int> &vector, const cv::Scalar &default)
{
    if (vector.size() != 3) {
        return default;
    }

    return cv::Scalar(vector[0], vector[1], vector[2]);
}
