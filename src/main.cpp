#include <iostream>

#include "Options.h"
#include "Window.h"
#include "Capture.h"
#include "FPS.h"

int main(int argc, char* argv[])
{
    Capture capture;
    auto screen = capture.Grab();

    if (screen.has_value()) {
        cv::imwrite("screen.bmp", screen.value());
    }

    FPS<100> fps;

    while (true) {
        fps.Begin();
        capture.Clear();

        auto rect = Window::Rect("Test");

        if (!rect.has_value()) {
            break;
        }

        auto window = capture.Grab(rect.value());

        if (!window.has_value()) {
            continue;
        }

        cv::imshow("l2-cv-bot", window.value());

        std::cout << fps.Get() << std::endl;

        if (cv::waitKey(1) == 27) {
            break;
        }
    }

    cv::destroyAllWindows();
    return 0;
}
