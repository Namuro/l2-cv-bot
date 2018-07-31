#pragma once

#include <algorithm>

#include <opencv2/opencv.hpp>

template <int Cap>
class FPS
{
    int64_t m_i;
    int64_t m_ticks;
    std::array<double, Cap> m_frame_times;

public:
    FPS() : m_i(0), m_ticks(0), m_frame_times() {}

    void Begin() { m_ticks = cv::getTickCount(); }
    double Get();
};

template <int Cap>
double FPS<Cap>::Get()
{
    // add frame time to ring buffer
    m_frame_times[m_i++ % m_frame_times.max_size()] = (cv::getTickCount() - m_ticks) / cv::getTickFrequency();

    // incrementally calculate average FPS
    double sum = 0;
    decltype(m_frame_times)::size_type count = 0;

    for (const auto time : m_frame_times) {
        if (time > 0) {
            sum += time;
            ++count;
        }
    }

    return 1 / (sum / count);
}
