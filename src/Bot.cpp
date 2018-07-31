#include "Bot.h"

void Bot::Run()
{
    auto esc_pressed = false;
    auto space_pressed = false;

    // register global key press callback
    m_input.RegisterKeyboardCallback([&esc_pressed, &space_pressed](int key) {
        switch (key) {
        case VK_ESCAPE:
            esc_pressed = true;
            break;
        case VK_SPACE:
            space_pressed = true;
            break;
        }
    });

    const auto title = m_options.String("--window", "Lineage II");
    const auto debug = m_options.Bool("--debug", true);
    auto foreground = false;

    while (true) {
        m_fps.Begin();
        m_capture.Clear();

        const auto window = Window::Find(title);

        if (!window.has_value()) {
            std::cout << "Can't find window \"" << title << "\"" << std::endl;
            break;
        }

        if (!foreground) {
            window.value().SetForeground();
            foreground = true;
        }

        const auto rect = window.value().Rect();
        const auto bitmap = m_capture.Grab(rect.x, rect.y, rect.width, rect.height);

        if (!bitmap.has_value()) {
            std::cout << "Failed to grab window" << std::endl;
            break;
        }

        const auto image = BitmapToImage(bitmap.value());

        if (!image.has_value()) {
            std::cout << "Failed to convert bitmap to image" << std::endl;
            break;
        }

        const auto world = m_eyes.Blink(image.value());

        if (debug) {
            if (world.has_value()) {
                DrawWorldInfo(image.value(), world.value());
            }

            const auto key = ShowDebugWindow(image.value());

            if (key == VK_ESCAPE) {
                esc_pressed = true;
            }
            else if (key == VK_SPACE) {
                space_pressed = true;
            }
        }

        if (esc_pressed || !debug && m_input.MouseMoved()) {
            std::cout << "Bye!" << std::endl;
            break;
        }
        else if (space_pressed) {
            m_eyes.Reset();
            space_pressed = false;
        }
    }

    cv::destroyAllWindows();
}

void Bot::DrawWorldInfo(const cv::Mat &image, const Eyes::World &world) const
{
    // draw help
    cv::putText(
        image,
        "Press Space to reset HP/MP/CP bars positions",
        cv::Point(5, image.rows - 215),
        cv::FONT_HERSHEY_COMPLEX,
        0.4,
        cv::Scalar(255, 255, 255),
        1,
        cv::LINE_AA
    );

    cv::putText(
        image,
        "Press ESC to exit",
        cv::Point(5, image.rows - 195),
        cv::FONT_HERSHEY_COMPLEX,
        0.4,
        cv::Scalar(255, 255, 255),
        1,
        cv::LINE_AA
    );

    // draw my HP/MP/CP values
    cv::putText(
        image,
        "My HP " + std::to_string(world.me.hp) + "% " +
        "MP " + std::to_string(world.me.mp) + "% " +
        "CP: " + std::to_string(world.me.cp) + "% ",
        cv::Point(5, 100),
        cv::FONT_HERSHEY_COMPLEX,
        0.5,
        cv::Scalar(0, 255, 255),
        1,
        cv::LINE_AA
    );

    // draw target HP value
    cv::putText(
        image,
        "Target HP " + std::to_string(world.target.hp) + "%",
        cv::Point(5, 125),
        cv::FONT_HERSHEY_COMPLEX,
        0.5,
        cv::Scalar(0, 255, 255),
        1,
        cv::LINE_AA
    );

    // draw NPCs debug info
    for (const auto &npc : world.npcs) {
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
}

int Bot::ShowDebugWindow(const cv::Mat &image)
{
    // draw target HP bar
    const auto target_hp_bar = m_eyes.TargetHPBar();

    if (target_hp_bar.has_value()) {
        cv::rectangle(image, target_hp_bar.value(), cv::Scalar(255, 0, 255), 1);
    }

    // draw my bars
    const auto my_bars = m_eyes.MyBars();

    if (my_bars.has_value()) {
        cv::rectangle(image, my_bars.value().hp_bar, cv::Scalar(0, 0, 255), 1);
        cv::rectangle(image, my_bars.value().mp_bar, cv::Scalar(255, 255, 0), 1);
        cv::rectangle(image, my_bars.value().cp_bar, cv::Scalar(0, 255, 255), 1);
    }

    // draw FPS
    cv::putText(
        image,
        std::to_string(static_cast<int>(m_fps.Get())),
        cv::Point(5, image.rows - 5),
        cv::FONT_HERSHEY_PLAIN,
        2,
        cv::Scalar(255, 255, 0),
        2,
        cv::LINE_AA
    );

    cv::imshow("l2-cv-bot", image);
    return cv::waitKey(1) & 0xff;
}