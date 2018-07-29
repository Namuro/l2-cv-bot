#include <iostream>
#include <opencv2/opencv.hpp>

#include "Options.h"
#include "Window.h"
#include "Capture.h"
#include "FPS.h"
#include "Eyes.h"

int main(int argc, char* argv[])
{
    const Options options(argc, argv);
    Capture capture;
    FPS<100> fps;
    Eyes eyes;

    const auto title = options.String("--window", "Lineage II");
    const auto debug = options.Bool("--debug", true);

    while (true) {
        fps.Begin();
        capture.Clear();

        const auto rect = Window::Rect(title);

        if (!rect.has_value()) {
            std::cout << "Can't find window \"" << title << "\"" << std::endl;
            break;
        }

        const auto window = capture.Grab(rect.value());

        if (!window.has_value()) {
            std::cout << "Failed to grab window" << std::endl;
            continue;
        }

        const auto image = window.value();

        eyes.Blink(image);

        const auto targets = eyes.Targets();

        if (!debug) {
            continue;
        }

        // draw targets debug info
        for (const auto &target : targets) {
            cv::rectangle(image, target.rect, cv::Scalar(255, 255, 0), 1);
            cv::circle(image, target.center, 10, cv::Scalar(0, 255, 255), 1);

            // draw target id
            std::stringstream ss;
            ss << "id" << target.id;
            cv::putText(image, ss.str(), cv::Point(target.rect.x, target.rect.y - 5), cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
        }

        // draw target HP bar rect
        const auto target_hp_bar = eyes.TargetHPBar();

        if (target_hp_bar.has_value()) {
            cv::rectangle(image, target_hp_bar.value(), cv::Scalar(255, 0, 255), 1);
        }

        // draw FPS
        std::stringstream ss;
        ss << std::floor(fps.Get());
        cv::putText(image, ss.str(), cv::Point(0, image.rows), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 255, 0), 2, cv::LINE_AA);

        cv::imshow("l2-cv-bot", image);

        const auto key = cv::waitKey(1) & 0xff;

        if (key == 27) { // 27 = ESC
            break;
        }
        else if (key == 32) { // 32 = Space
            eyes.Reset();
        }
    }

    cv::destroyAllWindows();
    return 0;
}
