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

class Bot
{
    Options m_options;
    Capture m_capture;
    FPS<100> m_fps;
    Eyes m_eyes;
    Input m_input;

public:
    Bot(int argc, char* argv[]) : m_options{argc, argv} {}

    void Run();

private:
    void DrawWorldInfo(const cv::Mat &image, const Eyes::World &world) const;
    int ShowDebugWindow(const cv::Mat &image);
    void ConfigureEyes();
};
