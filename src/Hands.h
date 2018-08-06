#pragma once

#include "Input.h"

class Hands : public Input
{
public:
    bool Ready();

    void ResetUI(); // ready after 1 sec
    void ResetCamera(); // ready after 1 sec
    void LookAround(); // ready when Input is ready
    void SelectTarget(); // ready when Input is ready
    void CancelTarget(); // ready after 1 sec

private:

};
