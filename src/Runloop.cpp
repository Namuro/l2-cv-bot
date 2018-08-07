#include "Utils.h"
#include "Window.h"

#include "Runloop.h"

void Runloop::Run()
{
    ConfigureEyes();

    const auto title = m_options.String("--window", "Lineage II");
    const auto debug = m_options.Bool("--debug", true);
    auto foreground = false;

    while (true) {
        m_capture.Clear();
        const auto window = ::Window::Find(title);

        if (!window.has_value()) {
            std::cout << "Can't find window \"" << title << "\"" << std::endl;
            break;
        }

        if (!foreground) {
            window.value().BringToForeground();
        }

        const auto rect = window.value().Rect();
        const auto bitmap = m_capture.Grab({rect.x, rect.y, rect.width, rect.height});

        m_hands.SetWindowRect({rect.x, rect.y, rect.width, rect.height});

        if (!foreground) {
            m_hands.ResetUI();
            m_hands.LookAround();
            m_hands.Send(500);
            foreground = true;
        }

        if (!m_hands.Ready()) {
            continue;
        }

        if (!bitmap.has_value()) {
            std::cout << "Failed to grab window" << std::endl;
            break;
        }

        auto image = BitmapToImage(bitmap.value()); // TODO: make const (OpenCV 4+)

        if (!image.has_value()) {
            std::cout << "Failed to convert bitmap to image" << std::endl;
            break;
        }

        if (m_hands.KeyboardKeyPressed(::Input::KeyboardKey::PrintScreen)) {
            cv::imwrite("shot.png", image.value());
            std::cout << "Screenshot saved to shot.png" << std::endl;
        }

        m_eyes.Blink(image.value());

        if (debug) {
            DrawWorldInfo(image.value());
            ShowDebugWindow(image.value());
        }

        if (!debug && m_hands.MouseMoved(100) || m_hands.KeyboardKeyPressed(::Input::KeyboardKey::Escape)) {
            std::cout << "Bye!" << std::endl;
            break;
        } else if (m_hands.KeyboardKeyPressed(::Input::KeyboardKey::Space)) {
            m_eyes.Reset();
        }
    }

    cv::destroyAllWindows();
}

void Runloop::DrawWorldInfo(cv::Mat &image) const
{
    const auto npcs = m_eyes.DetectNPCs();
    const auto me = m_eyes.DetectMe().value_or(::Eyes::Me());
    const auto target = m_eyes.DetectTarget().value_or(::Eyes::Target());

    // draw help
    cv::putText(
        image,
        "Press PrtScn to take screenshot of the Lineage II window",
        {5, image.rows - 235},
        cv::FONT_HERSHEY_COMPLEX,
        0.4,
        {255, 255, 255},
        1,
        cv::LINE_AA
    );

    cv::putText(
        image,
        "Press Space to reset HP/MP/CP bars position",
        {5, image.rows - 215},
        cv::FONT_HERSHEY_COMPLEX,
        0.4,
        {255, 255, 255},
        1,
        cv::LINE_AA
    );

    cv::putText(
        image,
        "Press ESC to exit",
        {5, image.rows - 195},
        cv::FONT_HERSHEY_COMPLEX,
        0.4,
        {255, 255, 255},
        1,
        cv::LINE_AA
    );

    // draw my HP/MP/CP values
    cv::putText(
        image,
        "My HP " + std::to_string(me.hp) + "% " +
        "MP " + std::to_string(me.mp) + "% " +
        "CP: " + std::to_string(me.cp) + "% ",
        {5, 100},
        cv::FONT_HERSHEY_COMPLEX,
        0.5,
        {0, 255, 255},
        1,
        cv::LINE_AA
    );

    // draw target HP value
    cv::putText(
        image,
        "Target HP " + std::to_string(target.hp) + "%",
        {5, 125},
        cv::FONT_HERSHEY_COMPLEX,
        0.5,
        {0, 255, 255},
        1,
        cv::LINE_AA
    );

    // draw NPCs debug info
    for (const auto &npc : npcs) {
        cv::rectangle(image, npc.rect, {255, 255, 0});
        cv::circle(image, npc.center, 10, {0, 255, 255});

        // draw NPC id
        cv::putText(
            image,
            "id" + std::to_string(npc.id),
            {npc.rect.x, npc.rect.y - 5},
            cv::FONT_HERSHEY_PLAIN,
            0.8,
            {255, 255, 255},
            1,
            cv::LINE_AA
        );
    }
}

int Runloop::ShowDebugWindow(cv::Mat &image)
{
    // draw target HP bar
    const auto target_hp_bar = m_eyes.TargetHPBar();

    if (target_hp_bar.has_value()) {
        cv::rectangle(image, target_hp_bar.value(), {255, 0, 255});
    }

    // draw my bars
    const auto my_bars = m_eyes.MyBars();

    if (my_bars.has_value()) {
        cv::rectangle(image, my_bars.value().hp_bar, {0, 0, 255});
        cv::rectangle(image, my_bars.value().mp_bar, {255, 255, 0});
        cv::rectangle(image, my_bars.value().cp_bar, {0, 255, 255});
    }

    // draw FPS
    cv::putText(
        image,
        std::to_string(static_cast<int>(m_fps.Get())),
        {5, image.rows - 5},
        cv::FONT_HERSHEY_PLAIN,
        2,
        {255, 255, 0},
        2,
        cv::LINE_AA
    );

    if (m_hands.KeyboardKeyPressed(::Input::KeyboardKey::F12)) {
        cv::imwrite("preview.bmp", image);
        std::cout << "Preview saved to preview.bmp" << std::endl;
    }

    cv::imshow("l2-cv-bot", image);
    return cv::waitKey(1) & 0xFF;
}

void Runloop::ConfigureEyes()
{
    // NPC detection
    m_eyes.m_npc_name_min_height        = m_options.Int("--npc_name_min_height", m_eyes.m_npc_name_min_height);
    m_eyes.m_npc_name_max_height        = m_options.Int("--npc_name_max_height", m_eyes.m_npc_name_max_height);
    m_eyes.m_npc_name_min_width         = m_options.Int("--npc_name_min_width", m_eyes.m_npc_name_min_width);
    m_eyes.m_npc_name_max_width         = m_options.Int("--npc_name_max_width", m_eyes.m_npc_name_max_width);
    m_eyes.m_npc_name_color_from_hsv    = ::VectorToScalar(m_options.IntVector("--npc_name_color_from_hsv"), m_eyes.m_npc_name_color_from_hsv);
    m_eyes.m_npc_name_color_to_hsv      = ::VectorToScalar(m_options.IntVector("--npc_name_color_to_hsv"), m_eyes.m_npc_name_color_to_hsv);
    m_eyes.m_npc_name_color_threshold   = m_options.Double("--npc_name_color_threshold", m_eyes.m_npc_name_color_threshold);
    m_eyes.m_npc_name_center_offset     = m_options.Int("--npc_name_center_offset", m_eyes.m_npc_name_center_offset);

    // current target detection
    m_eyes.m_target_circle_min_height       = m_options.Int("--target_circle_min_height", m_eyes.m_target_circle_min_height);
    m_eyes.m_target_circle_max_height       = m_options.Int("--target_circle_max_height", m_eyes.m_target_circle_max_height);
    m_eyes.m_target_circle_min_width        = m_options.Int("--target_circle_min_width", m_eyes.m_target_circle_min_width);
    m_eyes.m_target_circle_max_width        = m_options.Int("--target_circle_max_width", m_eyes.m_target_circle_max_width);
    m_eyes.m_target_circle_color_from_hsv   = ::VectorToScalar(m_options.IntVector("--target_circle_color_from_hsv"), m_eyes.m_target_circle_color_from_hsv);
    m_eyes.m_target_circle_color_to_hsv     = ::VectorToScalar(m_options.IntVector("--target_circle_color_to_hsv"), m_eyes.m_target_circle_color_to_hsv);
    m_eyes.m_target_center_offset           = m_options.Int("--target_center_offset", m_eyes.m_target_center_offset);

    // my HP/MP/CP bars detection
    m_eyes.m_my_bar_min_height      = m_options.Int("--my_bar_min_height", m_eyes.m_my_bar_min_height);
    m_eyes.m_my_bar_max_height      = m_options.Int("--my_bar_max_height", m_eyes.m_my_bar_max_height);
    m_eyes.m_my_bar_min_width       = m_options.Int("--my_bar_min_width", m_eyes.m_my_bar_min_width);
    m_eyes.m_my_bar_max_width       = m_options.Int("--my_bar_max_width", m_eyes.m_my_bar_max_width);
    m_eyes.m_my_hp_color_from_hsv   = ::VectorToScalar(m_options.IntVector("--my_hp_color_from_hsv"), m_eyes.m_my_hp_color_from_hsv);
    m_eyes.m_my_hp_color_to_hsv     = ::VectorToScalar(m_options.IntVector("--my_hp_color_to_hsv"), m_eyes.m_my_hp_color_to_hsv);
    m_eyes.m_my_mp_color_from_hsv   = ::VectorToScalar(m_options.IntVector("--my_mp_color_from_hsv"), m_eyes.m_my_mp_color_from_hsv);
    m_eyes.m_my_mp_color_to_hsv     = ::VectorToScalar(m_options.IntVector("--my_mp_color_to_hsv"), m_eyes.m_my_mp_color_to_hsv);
    m_eyes.m_my_cp_color_from_hsv   = ::VectorToScalar(m_options.IntVector("--my_cp_color_from_hsv"), m_eyes.m_my_cp_color_from_hsv);
    m_eyes.m_my_cp_color_to_hsv     = ::VectorToScalar(m_options.IntVector("--my_cp_color_to_hsv"), m_eyes.m_my_cp_color_to_hsv);

    // target HP bar detection
    m_eyes.m_target_hp_min_height       = m_options.Int("--target_hp_min_height", m_eyes.m_target_hp_min_height);
    m_eyes.m_target_hp_max_height       = m_options.Int("--target_hp_max_height", m_eyes.m_target_hp_max_height);
    m_eyes.m_target_hp_min_width        = m_options.Int("--target_hp_min_width", m_eyes.m_target_hp_min_width);
    m_eyes.m_target_hp_max_width        = m_options.Int("--target_hp_max_width", m_eyes.m_target_hp_max_width);
    m_eyes.m_target_hp_color_from_hsv   = ::VectorToScalar(m_options.IntVector("--target_hp_color_from_hsv"), m_eyes.m_target_hp_color_from_hsv);
    m_eyes.m_target_hp_color_to_hsv     = ::VectorToScalar(m_options.IntVector("--target_hp_color_to_hsv"), m_eyes.m_target_hp_color_to_hsv);
}
