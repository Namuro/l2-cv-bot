#include "Runloop.h"

#include "Utils.h"
#include "Window.h"

void Runloop::Run()
{
    ConfigureEyes();
    ConfigureHands();
    ConfigureBrain();

    const auto title = m_options.String("--window", "Lineage II");
    const auto debug = m_options.Bool("--debug", true);
    auto first = true;

    while (true) {
        m_capture.Clear();
        const auto window = ::Window::Find(title);

        if (!window.has_value()) {
            std::cout << "Can't find window \"" << title << "\"" << std::endl;
            break;
        }

        if (first) {
            window.value().BringToForeground();
        }

        const auto rect = window.value().Rect();
        const auto bitmap = m_capture.Grab({rect.x, rect.y, rect.width, rect.height});

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

        m_hands.SetWindowRect({rect.x, rect.y, rect.width, rect.height});
        m_eyes.Open(image.value());

        if (first) {
            m_brain.Init();
        }

        m_brain.Process();
        m_eyes.Close();

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

        first = false;
    }

    cv::destroyAllWindows();
}

void Runloop::DrawWorldInfo(cv::Mat &image) const
{
    const auto npcs = m_brain.NPCs();
    const auto far_npcs = m_brain.FarNPCs();
    const auto me = m_brain.Me().value_or(::Eyes::Me());
    const auto target = m_brain.Target().value_or(::Eyes::Target());

    // help
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

    // my HP/MP/CP
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

    // target HP
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

    // NPCs
    for (const auto &npc : npcs) {
        cv::rectangle(image, npc.rect, {255, 255, 0});
        cv::circle(image, npc.center, 10, {0, 255, 255});

        // name id
        cv::putText(
            image,
            "name id: " + std::to_string(npc.name_id),
            {npc.rect.x, npc.rect.y - 5},
            cv::FONT_HERSHEY_PLAIN,
            0.8,
            {255, 255, 255},
            1,
            cv::LINE_AA
        );

        // tracking id
        cv::putText(
            image,
            "tracking id: " + std::to_string(npc.tracking_id),
            {npc.rect.x, npc.rect.y - 20},
            cv::FONT_HERSHEY_PLAIN,
            0.8,
            {255, 255, 255},
            1,
            cv::LINE_AA
        );

        // selected
        cv::putText(
            image,
            "selected: " + std::to_string(npc.Selected()),
            {npc.rect.x, npc.rect.y - 35},
            cv::FONT_HERSHEY_PLAIN,
            0.8,
            {255, 255, 255},
            1,
            cv::LINE_AA
        );

        // hovered
        cv::putText(
            image,
            "hovered: " + std::to_string(npc.Hovered()),
            {npc.rect.x, npc.rect.y - 50},
            cv::FONT_HERSHEY_PLAIN,
            0.8,
            {255, 255, 255},
            1,
            cv::LINE_AA
        );
    }

    // far NPCs
    for (const auto &npc : far_npcs) {
        cv::rectangle(image, npc.rect, {0, 255, 255});

        cv::putText(
            image,
            std::to_string(npc.tracking_id),
            {npc.rect.x, npc.rect.y + 10},
            cv::FONT_HERSHEY_PLAIN,
            0.8,
            {0, 255, 255},
            1,
            cv::LINE_AA
        );
    }
}

int Runloop::ShowDebugWindow(cv::Mat &image)
{
    // target HP bar
    const auto target_hp_bar = m_eyes.TargetHPBar();

    if (target_hp_bar.has_value()) {
        cv::rectangle(image, target_hp_bar.value(), {255, 0, 255});
    }

    // my bars
    const auto my_bars = m_eyes.MyBars();

    if (my_bars.has_value()) {
        cv::rectangle(image, my_bars.value().hp_bar, {0, 0, 255});
        cv::rectangle(image, my_bars.value().mp_bar, {255, 255, 0});
        cv::rectangle(image, my_bars.value().cp_bar, {0, 255, 255});
    }

    // FPS
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
    m_eyes.m_blind_spot_radius      = m_options.Int("--blind_spot_radius", m_eyes.m_blind_spot_radius);
    m_eyes.m_npc_tracking_distance  = m_options.Int("--npc_tracking_distance", m_eyes.m_npc_tracking_distance);

    // NPC detection
    m_eyes.m_npc_name_min_height        = m_options.Int("--npc_name_min_height", m_eyes.m_npc_name_min_height);
    m_eyes.m_npc_name_max_height        = m_options.Int("--npc_name_max_height", m_eyes.m_npc_name_max_height);
    m_eyes.m_npc_name_min_width         = m_options.Int("--npc_name_min_width", m_eyes.m_npc_name_min_width);
    m_eyes.m_npc_name_max_width         = m_options.Int("--npc_name_max_width", m_eyes.m_npc_name_max_width);
    m_eyes.m_npc_name_color_from_hsv    = ::VectorToScalar(m_options.IntVector("--npc_name_color_from_hsv"), m_eyes.m_npc_name_color_from_hsv);
    m_eyes.m_npc_name_color_to_hsv      = ::VectorToScalar(m_options.IntVector("--npc_name_color_to_hsv"), m_eyes.m_npc_name_color_to_hsv);
    m_eyes.m_npc_name_color_threshold   = m_options.Double("--npc_name_color_threshold", m_eyes.m_npc_name_color_threshold);
    m_eyes.m_npc_name_center_offset     = m_options.Int("--npc_name_center_offset", m_eyes.m_npc_name_center_offset);

    // selected target detection
    m_eyes.m_target_circle_area_height      = m_options.Int("--target_circle_area_height", m_eyes.m_target_circle_area_height);
    m_eyes.m_target_circle_area_width       = m_options.Int("--target_circle_area_width", m_eyes.m_target_circle_area_width);
    m_eyes.m_target_gray_circle_color_bgr   = ::VectorToScalar(m_options.IntVector("--target_gray_circle_color_bgr"), m_eyes.m_target_gray_circle_color_bgr);
    m_eyes.m_target_blue_circle_color_bgr   = ::VectorToScalar(m_options.IntVector("--target_blue_circle_color_bgr"), m_eyes.m_target_blue_circle_color_bgr);
    m_eyes.m_target_red_circle_color_bgr    = ::VectorToScalar(m_options.IntVector("--target_red_circle_color_bgr"), m_eyes.m_target_red_circle_color_bgr);

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

void Runloop::ConfigureHands()
{
    m_hands.m_attack_key        = ::StringToKeyboardKey(m_options.String("--attack_key"), m_hands.m_attack_key);
    m_hands.m_next_target_key   = ::StringToKeyboardKey(m_options.String("--next_target_key"), m_hands.m_next_target_key);
    m_hands.m_spoil_key         = ::StringToKeyboardKey(m_options.String("--spoil_key"), m_hands.m_spoil_key);
    m_hands.m_sweep_key         = ::StringToKeyboardKey(m_options.String("--sweep_key"), m_hands.m_sweep_key);
    m_hands.m_pick_up_key       = ::StringToKeyboardKey(m_options.String("--pick_up_key"), m_hands.m_pick_up_key);
    m_hands.m_restore_hp_key    = ::StringToKeyboardKey(m_options.String("--restore_hp_key"), m_hands.m_restore_hp_key);
    m_hands.m_restore_mp_key    = ::StringToKeyboardKey(m_options.String("--restore_mp_key"), m_hands.m_restore_mp_key);
    m_hands.m_restore_cp_key    = ::StringToKeyboardKey(m_options.String("--restore_cp_key"), m_hands.m_restore_cp_key);
}

void Runloop::ConfigureBrain()
{
    m_brain.m_search_attempts = m_options.Int("--search_attempts", m_brain.m_search_attempts);
}
