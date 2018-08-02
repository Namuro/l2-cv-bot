#include "Hands.h"

void Hands::SmoothMouseMove(const Input::Point &point) const
{
    constexpr auto step = 30;
    constexpr auto delay = 10;

    const auto from = m_input.MousePosition();
    const auto distance = std::hypot(point.x - from.x, point.y - from.y);
    const auto steps = distance / step;
    const auto dx = (point.x - from.x) / steps;
    const auto dy = (point.y - from.y) / steps;

    for (int i = 0; i < steps; ++i) {
        m_input.MouseMove({static_cast<int>(from.x + i * dx), static_cast<int>(from.y + i * dy)}, delay);
    }

    m_input.MouseMove(point, delay);
}
