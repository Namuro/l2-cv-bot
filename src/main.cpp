#include <iostream>

#include <opencv2/opencv.hpp>

#include "Utils.h"
#include "Options.h"
#include "Window.h"
#include "Capture.h"
#include "FPS.h"
#include "Eyes.h"
#include "Input.h"
#include "Hands.h"

int main(int argc, char* argv[])
{
    //Input input;
    //input.MouseMove(100, 100);
    //input.MouseMove(200, 200);
    //input.MouseMove(300, 300);
    //input.Send();
    //Sleep(5000);
    //return 0;
    Input input;
    auto esc_pressed = false;
    auto space_pressed = false;

    // register global key press callback
    input.RegisterKeyCallback([&esc_pressed, &space_pressed](int key) {
        switch (key) {
        case VK_ESCAPE:
            esc_pressed = true;
            break;
        case VK_SPACE:
            space_pressed = true;
            break;
        }
    });

    const Options options(argc, argv);
    Capture capture;
    FPS<100> fps;
    Eyes eyes;

    const auto title = options.String("--window", "Lineage II");
    const auto debug = options.Bool("--debug", true);

    while (true) {
        fps.Begin();
        capture.Clear();

        const auto rect = Window::FindRect(title);

        if (!rect.has_value()) {
            std::cout << "Can't find window \"" << title << "\"" << std::endl;
            break;
        }

        const auto window = capture.Grab(
            rect.value().x,
            rect.value().y,
            rect.value().width,
            rect.value().height
        );

        if (!window.has_value()) {
            std::cout << "Failed to grab window" << std::endl;
            break;
        }

        const auto image = BitmapToImage(window.value());

        eyes.Blink(image);

        const auto npcs = eyes.NPCs();
        const auto target = eyes.Target();
        const auto me = eyes.Me();

        if (!debug) {
            continue;
        }

        // draw my HP/MP/CP & target HP values
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

        // draw NPCs debug info
        for (const auto &npc : npcs) {
            cv::rectangle(image, npc.rect, cv::Scalar(255, 255, 0), 1);
            cv::circle(image, npc.center, 10, cv::Scalar(0, 255, 255), 1);

            // draw NPC id
            cv::putText(
                image,
                "id" + std::to_string(npc.id),
                cv::Point(npc.rect.x, npc.rect.y - 5),
                cv::FONT_HERSHEY_PLAIN,
                0.8,
                cv::Scalar(255, 255, 255),
                1,
                cv::LINE_AA
            );
        }

        // draw target HP bar
        const auto target_hp_bar = eyes.TargetHPBar();

        if (target_hp_bar.has_value()) {
            cv::rectangle(image, target_hp_bar.value(), cv::Scalar(255, 0, 255), 1);
        }

        // draw my bars
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

        if (!debug && input.MouseMoved() || esc_pressed || key == 27) { // 27 = ESC
            std::cout << "Bye!" << std::endl;
            break;
        }
        else if (space_pressed || key == 32) { // 32 = Space
            eyes.Reset();
            space_pressed = false;
        }
    }

    cv::destroyAllWindows();
    return 0;
}
