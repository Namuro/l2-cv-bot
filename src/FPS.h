#pragma once

#include <chrono>

template <int Cap>
class FPS
{
public:
    FPS() :
        m_frame {0},
        m_time  {std::chrono::steady_clock::now()},
        m_times {}
    {}

    double Get();

private:
    std::int64_t m_frame;
    std::chrono::time_point<std::chrono::steady_clock> m_time;
    std::array<std::int64_t, Cap> m_times;
};

template <int Cap>
double FPS<Cap>::Get()
{
    // add frame time to ring buffer
    const auto now = std::chrono::steady_clock::now();
    const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_time).count();
    m_times[m_frame++ % m_times.size()] = diff;
    m_time = now;

    // incrementally calculate average FPS
    decltype(m_times)::size_type count = 0;
    auto average = 0.0;

    for (const auto time : m_times) {
        if (time <= 0) {
            continue;
        }

        average = (time + average * count) / (count + 1);
        ++count;
    }

    return 1 / average * 1000;
}
