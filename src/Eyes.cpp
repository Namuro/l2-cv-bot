#include "Eyes.h"

void Eyes::Blink(const cv::Mat &rgb)
{
    cv::Mat hsv;
    cv::cvtColor(rgb, hsv, cv::COLOR_BGR2HSV);

    // detect targets
    if (!m_target.has_value()) {
        m_targets = DetectTargets(hsv);
    }

    // detect target HP bar
    if (!m_target_hp_bar.has_value()) {
        m_target_hp_bar = DetectTargetHPBar(hsv);
    }

    // detect my bars
    if (!m_my_bars.has_value()) {
        m_my_bars = DetectMyBars(hsv);
    }
}

void Eyes::Reset()
{
    m_my_bars = {};
    m_target_hp_bar = {};
}

std::vector<Target> Eyes::DetectTargets(const cv::Mat &hsv) const
{
    // TL;DR: search for NPC names

    // extract white regions (target names)
    cv::Mat white;
    cv::inRange(hsv, m_target_color_from_hsv, m_target_color_to_hsv, white);

    // increase white regions size
    cv::Mat mask;
    auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(white, mask, kernel);

    // join words in target names
    kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(21, 5));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

    // remove noise
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel, cv::Point(-1, -1), 2);

    // find external contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<struct Target> targets;

    // find correct contours
    for (const auto &contour : contours) {
        const auto rect = cv::boundingRect(contour);

        // check rect size & proportions
        if (rect.height < m_target_min_height || rect.height > m_target_max_height ||
            rect.width < m_target_min_width || rect.width > m_target_max_width ||
            rect.width < rect.height * 2
        ) {
            continue;
        }

        const auto target_image = white(rect);
        const auto threshold = cv::countNonZero(target_image) / target_image.total();

        if (threshold > m_target_color_threshold) {
            continue;
        }

        struct Target target = {};
        target.rect = rect;
        target.center = cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2 + 20);
        target.id = Hash(target_image);
        targets.push_back(target);
    }

    return targets;
}

std::optional<cv::Rect> Eyes::DetectTargetHPBar(const cv::Mat &hsv) const
{
    // TL;DR: search for long thin red bar

    // exract bar color
    cv::Mat mask;
    cv::inRange(hsv, m_target_hp_color_from_hsv, m_target_hp_color_to_hsv, mask);
    
    // remove noise
    const auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(m_target_hp_min_width, m_target_hp_min_height));
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel);

    // find external contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // find correct contour
    for (const auto &contour : contours) {
        const auto rect = cv::boundingRect(contour);

        // check rect size & proportions
        if (rect.height < m_target_hp_min_height || rect.height > m_target_hp_max_height ||
            rect.width < m_target_hp_min_width || rect.width > m_target_hp_max_width
        ) {
            continue;
        }

        return rect;
    }
    
    return {};
}

std::optional<MyBars> Eyes::DetectMyBars(const cv::Mat &hsv) const
{
    // TL;DR: search for HP bar, then detect CP bar above and MP bar below

    // exract HP bar color
    cv::Mat mask;
    cv::inRange(hsv, m_my_hp_color_from_hsv, m_my_hp_color_to_hsv, mask);

    const auto contours = BarContours(mask);

    // search for CP bar above and MP bar below
    for (const auto &contour : contours) {
        const auto hp_rect = cv::boundingRect(contour);

        // check rect size & proportions
        if (hp_rect.height < m_my_bar_min_height || hp_rect.height > m_my_bar_max_height ||
            hp_rect.width < m_my_bar_min_width || hp_rect.width > m_my_bar_max_width
        ) {
            continue;
        }

        // search other bars near HP bar
        const auto bars_rect = hp_rect + cv::Point(0, -hp_rect.height * 2) + cv::Size(0, hp_rect.height * 4);
        const auto bars = hsv(bars_rect);

        cv::Mat mp;
        cv::inRange(bars, m_my_mp_color_from_hsv, m_my_mp_color_to_hsv, mp);

        cv::Mat cp;
        cv::inRange(bars, m_my_cp_color_from_hsv, m_my_cp_color_to_hsv, cp);

        cv::Mat mp_cp;
        cv::bitwise_or(cp, mp, mp_cp);

        const auto bar_contours = BarContours(mp_cp);

        // no CP or MP bar found
        if (bar_contours.size() != 2) {
            continue;
        }

        struct MyBars my_bars;
        my_bars.hp_bar = hp_rect;
        my_bars.mp_bar = cv::boundingRect(bar_contours[1]) + bars_rect.tl();
        my_bars.cp_bar = cv::boundingRect(bar_contours[0]) + bars_rect.tl();
        return my_bars;
    }

    return {};
}

std::vector<std::vector<cv::Point>> Eyes::BarContours(const cv::Mat &mask) const
{
    // remove noise
    auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, m_my_bar_min_height));
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel);

    // join parts of the bar
    kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(25, 1));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

    // find external contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    return contours;
}

uint32_t Eyes::Hash(const cv::Mat &image)
{
    // djb2 hash
    uint32_t hash = 5381;

    for (size_t i = 0; i < image.total(); i++) {
        hash = ((hash << 5) + hash) ^ *(image.data + i);
    }

    return hash;
}
