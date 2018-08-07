#pragma once

#include <iostream>

#include <opencv2/opencv.hpp>

#include "Options.h"
#include "Capture.h"
#include "FPS.h"
#include "Eyes.h"
#include "Hands.h"

class Runloop
{
public:
    Runloop(int argc, char* argv[]) : m_options{argc, argv} {} // throws

    void Run();
private:
    ::Options m_options;
    ::Capture m_capture;
    ::FPS<100> m_fps;
    ::Eyes m_eyes;
    ::Hands m_hands;

    void DrawWorldInfo(cv::Mat &image) const;
    int ShowDebugWindow(cv::Mat &image);
    void ConfigureEyes();
};
