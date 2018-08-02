#pragma once

#include <chrono>

template <int Cap>
class FPS
{
    std::int64_t m_i;
    std::chrono::time_point<std::chrono::steady_clock> m_time;
    std::array<std::int64_t, Cap> m_times;

public:
    FPS() : m_i{0}, m_time{std::chrono::steady_clock::now()}, m_times{} {}

    double Get();
};

template <int Cap>
double FPS<Cap>::Get()
{
    // add frame time to ring buffer
    const auto now = std::chrono::steady_clock::now();
    const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_time).count();
    m_times[m_i++ % m_times.max_size()] = diff;
    m_time = now;

    // incrementally calculate average FPS
    decltype(m_times)::size_type count = 0;
    double average = 0.0;

    for (const auto time : m_times) {
        if (time <= 0) {
            continue;
        }

        average = (time + average * count) / (count + 1);
        ++count;
    }

    return 1 / average * 1000;
}
