#include <iostream>
#include <opencv2/opencv.hpp>

#include "Options.h"
#include "Window.h"
#include "Capture.h"
#include "FPS.h"
#include "Eyes.h"

int main(int argc, char* argv[])
{
    Options options(argc, argv);
    Capture capture;
    FPS<100> fps;
    Eyes eyes;

    auto title = options.String("--window", "Lineage II");
    auto debug = options.Bool("--debug", true);

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

        auto image = window.value();
        eyes.Blink(image);

        if (!debug) {
            continue;
        }

        auto targets = eyes.Targets();

        for (auto &target : targets) {
            cv::rectangle(image, target.rect, cv::Scalar(255, 255, 0), 1);
            cv::circle(image, target.center, 10, cv::Scalar(0, 255, 255), 1);

            // target id
            std::stringstream ss;
            ss << "id" << target.id;
            cv::putText(image, ss.str(), cv::Point(target.rect.x, target.rect.y - 5), cv::FONT_HERSHEY_PLAIN, 0.75, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
        }

        // fps
        std::stringstream ss;
        ss << std::floor(fps.Get());
        cv::putText(image, ss.str(), cv::Point(0, image.rows), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 255, 0), 2, cv::LINE_AA);

        cv::imshow("l2-cv-bot", image);

        if (cv::waitKey(1) == 27) {
            break;
        }
    }

    cv::destroyAllWindows();
    return 0;
}
