#pragma once

#include <optional>

#include <opencv2/opencv.hpp>

#include "Capture.h"
#include "Input.h"

#define LOCKED(ms) Locked(ms, __FILE__, __LINE__)

std::optional<cv::Mat> BitmapToImage(const ::Capture::Bitmap &bitmap);
cv::Scalar VectorToScalar(const std::vector<int> &vector, const cv::Scalar &default);
::Input::KeyboardKey StringToKeyboardKey(const std::string &string, ::Input::KeyboardKey default);
bool Locked(int ms, const std::string &file, int line);
