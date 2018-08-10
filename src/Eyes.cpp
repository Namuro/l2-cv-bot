#include <limits>

#include "Eyes.h"

std::vector<Eyes::NPC> Eyes::DetectNPCs() const
{
    // extract regions with white NPC names
    cv::Mat white;
    cv::inRange(m_hsv, m_npc_name_color_from_hsv, m_npc_name_color_to_hsv, white);

    // increase white regions size
    cv::Mat mask;
    auto kernel = cv::getStructuringElement(cv::MORPH_RECT, {3, 3});
    cv::dilate(white, mask, kernel);

    // join words
    kernel = cv::getStructuringElement(cv::MORPH_RECT, {17, 5});
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

    // remove noise
    kernel = cv::getStructuringElement(cv::MORPH_RECT, {11, 5});
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<NPC> npcs;
    auto selected = false;

    for (const auto &contour : contours) {
        const auto rect = cv::boundingRect(contour);

        if (rect.height < m_npc_name_min_height || rect.height > m_npc_name_max_height ||
            rect.width < m_npc_name_min_width || rect.width > m_npc_name_max_width ||
            rect.width < rect.height * 2
        ) {
            continue;
        }

        const auto target_image = white(rect);
        const auto threshold = cv::countNonZero(target_image) / target_image.total();

        if (threshold > m_npc_name_color_threshold) {
            continue;
        }

        NPC npc = {};
        npc.rect = rect;
        npc.center = {rect.x + rect.width / 2, rect.y + rect.height / 2 + m_npc_name_center_offset};
        npc.name_id = Hash(target_image);
        npc.center_id = npc.center.x << 16 | npc.center.y;
        npc.selected = !selected ? NPCSelected(rect) : false; // only one NPC can be selected
        npcs.push_back(npc);

        selected = npc.selected;
    }

    // remove myself from NPCs
    auto min_x = std::numeric_limits<int>::max();
    auto min_y = min_x;
    decltype(npcs)::const_iterator min_it = npcs.end();

    for (auto it = npcs.begin(); it != npcs.end(); ++it) {
        const auto npc = *it;
        const auto x = std::abs(npc.center.x - m_hsv.cols / 2);
        const auto y = std::abs(npc.center.y - m_hsv.rows / 2);

        if (x < min_x || y < min_y) {
            min_x = x;
            min_y = y;
            min_it = it;
        }
    }

    if (min_it != npcs.end()) {
        npcs.erase(min_it);
    }

    return npcs;
}

std::optional<Eyes::Me> Eyes::DetectMe() const
{
    if (!m_my_bars.has_value()) {
        return {};
    }

    Me me = {};
    me.hp = CalcBarPercentValue(m_hsv(m_my_bars.value().hp_bar), m_my_hp_color_from_hsv, m_my_hp_color_to_hsv);
    me.mp = CalcBarPercentValue(m_hsv(m_my_bars.value().mp_bar), m_my_mp_color_from_hsv, m_my_mp_color_to_hsv);
    me.cp = CalcBarPercentValue(m_hsv(m_my_bars.value().cp_bar), m_my_cp_color_from_hsv, m_my_cp_color_to_hsv);
    return me;
}

std::optional<Eyes::Target> Eyes::DetectTarget() const
{
    if (!m_target_hp_bar.has_value()) {
        return {};
    }

    Target target = {};

    target.hp = CalcBarPercentValue(
        m_hsv(m_target_hp_bar.value()),
        m_target_hp_color_from_hsv,
        m_target_hp_color_to_hsv
    );

    return target;
}

std::optional<struct Eyes::MyBars> Eyes::DetectMyBars() const
{
    // extract red regions with red HP bar
    cv::Mat mask;
    cv::inRange(m_hsv, m_my_hp_color_from_hsv, m_my_hp_color_to_hsv, mask);

    const auto contours = FindMyBarContours(mask);

    // search for CP bar above and MP bar below
    for (const auto &contour : contours) {
        const auto rect = cv::boundingRect(contour);

        if (rect.height < m_my_bar_min_height || rect.height > m_my_bar_max_height ||
            rect.width < m_my_bar_min_width || rect.width > m_my_bar_max_width
        ) {
            continue;
        }

        // expand rect
        const auto bars_rect = rect + cv::Size{0, rect.height * 4} + cv::Point{0, -rect.height * 2};

        if (!RectInImage(m_hsv, bars_rect)) {
            continue;
        }

        const auto bars = m_hsv(bars_rect);

        // extract blue & yellow regions
        cv::Mat mp;
        cv::inRange(bars, m_my_mp_color_from_hsv, m_my_mp_color_to_hsv, mp);
        cv::Mat cp;
        cv::inRange(bars, m_my_cp_color_from_hsv, m_my_cp_color_to_hsv, cp);
        cv::Mat mp_cp;
        cv::bitwise_or(cp, mp, mp_cp);

        const auto bar_contours = FindMyBarContours(mp_cp);

        // no CP nor MP bar were found
        if (bar_contours.size() != 2) {
            continue;
        }

        struct MyBars my_bars = {};
        my_bars.hp_bar = rect;
        my_bars.mp_bar = cv::boundingRect(bar_contours[0]) + bars_rect.tl();
        my_bars.cp_bar = cv::boundingRect(bar_contours[1]) + bars_rect.tl();
        return my_bars;
    }

    return {};
}

std::optional<cv::Rect> Eyes::DetectTargetHPBar() const
{
    // extract red regions with red HP bar
    cv::Mat mask;
    cv::inRange(m_hsv, m_target_hp_color_from_hsv, m_target_hp_color_to_hsv, mask);
    
    // remove noise
    const auto kernel = cv::getStructuringElement(cv::MORPH_RECT, {25, m_target_hp_min_height});
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (const auto &contour : contours) {
        const auto rect = cv::boundingRect(contour);

        if (rect.height < m_target_hp_min_height || rect.height > m_target_hp_max_height ||
            rect.width < m_target_hp_min_width || rect.width > m_target_hp_max_width
        ) {
            continue;
        }

        return rect;
    }

    return {};
}

std::vector<std::vector<cv::Point>> Eyes::FindMyBarContours(const cv::Mat &mask) const
{
    // remove noise
    auto kernel = cv::getStructuringElement(cv::MORPH_RECT, {1, m_my_bar_min_height});
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel);

    // join parts of the bar
    kernel = cv::getStructuringElement(cv::MORPH_RECT, {25, 1});
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    return contours;
}

bool Eyes::NPCSelected(const cv::Rect &rect) const
{
    // expand rect
    const auto expanded_rect = rect +
        cv::Size(m_target_circle_area_width * 2, m_target_circle_area_height - rect.height) +
        cv::Point(-m_target_circle_area_width, -(m_target_circle_area_height - rect.height) / 2);
    
    if (!RectInImage(m_hsv, expanded_rect)) {
        return false;
    }

    cv::Mat hsv = m_hsv(expanded_rect);
    cv::rectangle(hsv, {m_target_circle_area_width, 0, rect.width, m_target_circle_area_height}, 0, -1);

    // extract blue & red regions
    cv::Mat blue;
    cv::inRange(hsv, m_target_blue_circle_color_from_hsv, m_target_blue_circle_color_to_hsv, blue);
    cv::Mat red;
    cv::inRange(hsv, m_target_red_circle_color_from_hsv, m_target_red_circle_color_to_hsv, red);
    cv::Mat mask;
    cv::bitwise_or(blue, red, mask);

    // increase regions size
    const auto kernel = cv::getStructuringElement(cv::MORPH_RECT, {5, 5});
    cv::dilate(mask, mask, kernel);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    if (contours.size() < 2) {
        return false;
    }

    // compare each contour to find pair
    for (const auto &contour1 : contours) {
        for (const auto &contour2 : contours) {
            if (contour1 == contour2) {
                continue;
            }

            const auto rect1 = cv::boundingRect(contour1);
            const auto rect2 = cv::boundingRect(contour2);

            if (rect1.y != rect2.y || rect1.size() != rect2.size()) {
                continue;
            }

            return true;
        }
    }

    return false;
}

int Eyes::CalcBarPercentValue(const cv::Mat &bar, const cv::Scalar &from_color, const cv::Scalar &to_color)
{
    CV_Assert(bar.rows >= 1);
    CV_Assert(bar.depth() == CV_8U);
    CV_Assert(bar.channels() >= 3);

    // loop mid row until first pixel with color in desired range
    const auto row = bar.ptr<uchar>(bar.rows / 2);
    auto channel = (bar.cols - 1) * bar.channels();
    auto cols = bar.cols;

    for (; channel > 0; channel -= bar.channels(), cols--) {
        if (row[channel + 0] >= from_color[0] && row[channel + 0] <= to_color[0] &&
            row[channel + 1] >= from_color[1] && row[channel + 1] <= to_color[1] &&
            row[channel + 2] >= from_color[2] && row[channel + 2] <= to_color[2]
        ) {
            break;
        }
    }

    return cols * 100 / bar.cols;
}

std::uint32_t Eyes::Hash(const cv::Mat &image)
{
    // djb2 hash
    std::uint32_t hash = 5381;
    const auto total = image.total();

    for (std::size_t i = 0; i < total; ++i) {
        hash = ((hash << 5) + hash) ^ *(image.data + i);
    }

    return hash;
}
