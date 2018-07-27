#include <iostream>

#include "Window.h"
#include "Capture.h"

int main(int argc, char* argv[])
{
    Capture capture;
    auto rect = Window::Rect("Test");
    auto screen = capture.Grab();

    if (screen.has_value()) {
        cv::imwrite("screen.bmp", screen.value());
    }

    if (rect.has_value()) {
        auto window = capture.Grab(rect.value());

        if (window.has_value()) {
            cv::imwrite("window.bmp", window.value());
        }
    }
}
