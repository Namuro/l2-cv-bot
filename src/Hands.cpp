#include "Hands.h"

#include <iostream>

void Hands::SmoothMouseMove(const Input::Point &point) const
{
    // calculate distance between source and destination points
    // calculate steps = divide distance by 50 pixels, then floor
    // xs = dx / step; ys = dy / step;
    // just increment x by xs, y by ys
    const auto from = m_input.MousePosition();
}
