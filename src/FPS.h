#pragma once

#include <opencv2/opencv.hpp>
#include <algorithm>

template <int Cap>
class FPS
{
    using frames = std::array<double, Cap>;

    int64_t m_i = 0;
    int64_t m_ticks = 0;
    frames m_frames = {};

public:
    void Begin() { m_ticks = cv::getTickCount(); }

    double Get()
    {
        // add frame time to ring buffer
        m_frames[m_i++ % m_frames.max_size()] = (cv::getTickCount() - m_ticks) / cv::getTickFrequency();

        // calculate average FPS
        double sum = 0;
        frames::size_type count = 0;

        for (const auto &time : m_frames) {
            if (time > 0) {
                sum += time;
                ++count;
            }
        }

        return 1 / (sum / count);
    }
};
