#pragma once

#include <iostream>
#include <optional>

#include <opencv2/opencv.hpp>

#include "Utils.h"
#include "Options.h"
#include "Window.h"
#include "Capture.h"
#include "FPS.h"
#include "Eyes.h"
#include "Input.h"
#include "Hands.h"

class Bot
{
    Options m_options;
    Capture m_capture;
    FPS<100> m_fps;
    Eyes m_eyes;
    Input m_input;
    Hands m_hands;

public:
    Bot(int argc, char* argv[]) : m_options(argc, argv), m_hands(m_input) {}

    void Run();

private:
    void DrawWorldInfo(const cv::Mat &image, const Eyes::World &world) const;
    int ShowDebugWindow(const cv::Mat &image);
    void ConfigureEyes();
};
