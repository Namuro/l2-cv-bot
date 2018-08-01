#include "Bot.h"

void Bot::Run()
{
    ConfigureEyes();

    const auto title = m_options.String("--window", "Lineage II");
    const auto debug = m_options.Bool("--debug", true);
    auto foreground = false;

    while (true) {
        m_capture.Clear();
        const auto window = Window::Find(title);

        if (!window.has_value()) {
            std::cout << "Can't find window \"" << title << "\"" << std::endl;
            break;
        }

        if (!foreground) {
            window.value().BringToForeground();
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

        if (m_input.KeyboardKeyPressed(VK_SNAPSHOT)) {
            cv::imwrite("shot.png", image.value());
        }

        const auto world = m_eyes.Blink(image.value());

        if (debug) {
            if (world.has_value()) {
                DrawWorldInfo(image.value(), world.value());
            }

            ShowDebugWindow(image.value());
        }

        if (m_input.KeyboardKeyPressed(VK_ESCAPE) || !debug && m_input.MouseMoved()) {
            std::cout << "Bye!" << std::endl;
            break;
        }
        else if (m_input.KeyboardKeyPressed(VK_SPACE)) {
            m_eyes.Reset();
        }
    }

    cv::destroyAllWindows();
}

void Bot::DrawWorldInfo(const cv::Mat &image, const Eyes::World &world) const
{
    // draw help
    cv::putText(
        image,
        "Press PrtScn to take screenshot of the Lineage II window",
        cv::Point(5, image.rows - 235),
        cv::FONT_HERSHEY_COMPLEX,
        0.4,
        cv::Scalar(255, 255, 255),
        1,
        cv::LINE_AA
    );

    cv::putText(
        image,
        "Press Space to reset HP/MP/CP bars position",
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

    if (m_input.KeyboardKeyPressed(VK_F12)) {
        cv::imwrite("preview.bmp", image);
    }

    cv::imshow("l2-cv-bot", image);
    return cv::waitKey(1) & 0xff;
}

void Bot::ConfigureEyes()
{
    // NPC detection
    m_eyes.m_npc_name_min_height        = m_options.Int("--npc_name_min_height", m_eyes.m_npc_name_min_height);
    m_eyes.m_npc_name_max_height        = m_options.Int("--npc_name_max_height", m_eyes.m_npc_name_max_height);
    m_eyes.m_npc_name_min_width         = m_options.Int("--npc_name_min_width", m_eyes.m_npc_name_min_width);
    m_eyes.m_npc_name_max_width         = m_options.Int("--npc_name_max_width", m_eyes.m_npc_name_max_width);
    m_eyes.m_npc_name_color_from_hsv    = VectorToScalar(m_options.IntVector("--npc_name_color_from_hsv"), m_eyes.m_npc_name_color_from_hsv);
    m_eyes.m_npc_name_color_to_hsv      = VectorToScalar(m_options.IntVector("--npc_name_color_to_hsv"), m_eyes.m_npc_name_color_to_hsv);
    m_eyes.m_npc_name_color_threshold   = m_options.Double("--npc_name_color_threshold", m_eyes.m_npc_name_color_threshold);

    // my HP/MP/CP bars detection
    m_eyes.m_my_bar_min_height      = m_options.Int("--my_bar_min_height", m_eyes.m_my_bar_min_height);
    m_eyes.m_my_bar_max_height      = m_options.Int("--my_bar_max_height", m_eyes.m_my_bar_max_height);
    m_eyes.m_my_bar_min_width       = m_options.Int("--my_bar_min_width", m_eyes.m_my_bar_min_width);
    m_eyes.m_my_bar_max_width       = m_options.Int("--my_bar_max_width", m_eyes.m_my_bar_max_width);
    m_eyes.m_my_hp_color_from_hsv   = VectorToScalar(m_options.IntVector("--my_hp_color_from_hsv"), m_eyes.m_my_hp_color_from_hsv);
    m_eyes.m_my_hp_color_to_hsv     = VectorToScalar(m_options.IntVector("--my_hp_color_to_hsv"), m_eyes.m_my_hp_color_to_hsv);
    m_eyes.m_my_mp_color_from_hsv   = VectorToScalar(m_options.IntVector("--my_mp_color_from_hsv"), m_eyes.m_my_mp_color_from_hsv);
    m_eyes.m_my_mp_color_to_hsv     = VectorToScalar(m_options.IntVector("--my_mp_color_to_hsv"), m_eyes.m_my_mp_color_to_hsv);
    m_eyes.m_my_cp_color_from_hsv   = VectorToScalar(m_options.IntVector("--my_cp_color_from_hsv"), m_eyes.m_my_cp_color_from_hsv);
    m_eyes.m_my_cp_color_to_hsv     = VectorToScalar(m_options.IntVector("--my_cp_color_to_hsv"), m_eyes.m_my_cp_color_to_hsv);

    // target HP bar detection
    m_eyes.m_target_hp_min_height       = m_options.Int("--target_hp_min_height", m_eyes.m_target_hp_min_height);
    m_eyes.m_target_hp_max_height       = m_options.Int("--target_hp_max_height", m_eyes.m_target_hp_max_height);
    m_eyes.m_target_hp_min_width        = m_options.Int("--target_hp_min_width", m_eyes.m_target_hp_min_width);
    m_eyes.m_target_hp_max_width        = m_options.Int("--target_hp_max_width", m_eyes.m_target_hp_max_width);
    m_eyes.m_target_hp_color_from_hsv   = VectorToScalar(m_options.IntVector("--target_hp_color_from_hsv"), m_eyes.m_target_hp_color_from_hsv);
    m_eyes.m_target_hp_color_to_hsv     = VectorToScalar(m_options.IntVector("--target_hp_color_to_hsv"), m_eyes.m_target_hp_color_to_hsv);
}
