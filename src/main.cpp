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
            break;
        }

        const auto image = window.value();

        eyes.Blink(image);

        const auto possible_targets = eyes.PossibleTargets();
        const auto target = eyes.Target();
        const auto me = eyes.Me();

        if (!debug) {
            continue;
        }

        // draw my HP/MP/CP values & target HP
        cv::putText(
            image,
            "My HP " + std::to_string(me.hp) + "% " +
            "MP " + std::to_string(me.mp) + "% " +
            "CP: " + std::to_string(me.cp) + "% "
            "Target HP " + std::to_string(target.hp) + "%",
            cv::Point(0, 100),
            cv::FONT_HERSHEY_COMPLEX,
            0.5,
            cv::Scalar(0, 255, 255),
            1,
            cv::LINE_AA
        );

        // draw targets debug info
        for (const auto &possible_target : possible_targets) {
            cv::rectangle(image, possible_target.rect, cv::Scalar(255, 255, 0), 1);
            cv::circle(image, possible_target.center, 10, cv::Scalar(0, 255, 255), 1);

            // draw target id
            cv::putText(
                image,
                "id" + std::to_string(possible_target.id),
                cv::Point(possible_target.rect.x, possible_target.rect.y - 5),
                cv::FONT_HERSHEY_PLAIN,
                0.8,
                cv::Scalar(255, 255, 255),
                1,
                cv::LINE_AA
            );
        }

        // draw target HP bar rect
        const auto target_hp_bar = eyes.TargetHPBar();

        if (target_hp_bar.has_value()) {
            cv::rectangle(image, target_hp_bar.value(), cv::Scalar(255, 0, 255), 1);
        }

        // draw my bars rects
        const auto my_bars = eyes.MyBars();

        if (my_bars.has_value()) {
            cv::rectangle(image, my_bars.value().hp_bar, cv::Scalar(0, 0, 255), 1);
            cv::rectangle(image, my_bars.value().mp_bar, cv::Scalar(255, 255, 0), 1);
            cv::rectangle(image, my_bars.value().cp_bar, cv::Scalar(0, 255, 255), 1);
        }

        // draw FPS
        cv::putText(
            image,
            std::to_string(static_cast<int>(fps.Get())),
            cv::Point(0, image.rows),
            cv::FONT_HERSHEY_PLAIN,
            2,
            cv::Scalar(255, 255, 0),
            2,
            cv::LINE_AA
        );

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
