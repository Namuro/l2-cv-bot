#pragma once

#include <optional>

#include <opencv2/opencv.hpp>

#include "Capture.h"
#include "Input.h"

std::optional<cv::Mat> BitmapToImage(const Capture::Bitmap &bitmap);
cv::Scalar VectorToScalar(const std::vector<int> &vector, const cv::Scalar &default);
::Input::KeyboardKey StringToKeyboardKey(const std::string &string, ::Input::KeyboardKey default);
