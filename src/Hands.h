#pragma once

#include "Input.h"

class Hands
{
    Input m_input;

public:
    bool Ready() const { return m_input.Ready(); }

private:
    void SmoothMouseMove(int x, int y);
};
