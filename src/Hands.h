#pragma once

#include "Input.h"

class Hands
{
    Input &m_input;

public:
    explicit Hands(Input &input) : m_input{input} {}

    bool Ready() const { return m_input.Ready(); }
    void ResetUI() const { /* Alt + L */ }
    void ResetCamera() const { /* move mouse to center & press RMB */ }

private:
    void SmoothMouseMove(const Input::Point &point) const;
};
