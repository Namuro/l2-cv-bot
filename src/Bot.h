#pragma once

#include <iostream>
#include <optional>

#include <opencv2/opencv.hpp>
#include <Windows.h>

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
    Input m_input;
    Capture m_capture;
    FPS<100> m_fps;
    Eyes m_eyes;

public:
    Bot(int argc, char* argv[]) : m_options(argc, argv) {}

    void Run();

private:
    void DrawWorldInfo(const cv::Mat &image, const Eyes::World &world) const;
    int ShowDebugWindow(const cv::Mat &image);
    void ConfigureEyes();
};
