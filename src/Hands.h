#pragma once

#include "Input.h"

class Hands
{
    Input &m_input;

public:
    explicit Hands(Input &input) : m_input(input) {}
    bool Ready() const { return m_input.Ready(); }

private:
    void SmoothMouseMove(const Input::Point &point) const;
};
