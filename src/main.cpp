#include <iostream>

#include "Options.h"
#include "Window.h"
#include "Capture.h"
#include "FPS.h"

int main(int argc, char* argv[])
{
    Options options(argc, argv);
    Capture capture;
    FPS<100> fps;

    auto title = options.String("--window", "Lineage II");

    while (true) {
        fps.Begin();
        capture.Clear();

        auto rect = Window::Rect(title);

        if (!rect.has_value()) {
            std::cout << "Can't find window \"" << title << "\"" << std::endl;
            break;
        }

        auto window = capture.Grab(rect.value());

        if (!window.has_value()) {
            std::cout << "Failed to grab window" << std::endl;
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
