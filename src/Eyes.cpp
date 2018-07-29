#include "Eyes.h"

void Eyes::Blink(const cv::Mat &rgb)
{
    cv::Mat hsv;
    cv::cvtColor(rgb, hsv, cv::COLOR_BGR2HSV);

    // detect targets
    m_targets = DetectTargets(hsv);

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
    for (auto &contour : contours) {
        const auto rect = cv::boundingRect(contour);

        // check contour size & proportions
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
    auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(m_target_hp_min_width, m_target_hp_min_height));
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel);

    // find external contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (auto &contour : contours) {
        const auto rect = cv::boundingRect(contour);

        // check contour size & proportions
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
    // TL;DR: search for HP bar, then traverse all possible rects and detect MP bar below and CP bar above

    // exract HP bar color
    cv::Mat mask;
    cv::inRange(hsv, m_my_hp_color_from_hsv, m_my_hp_color_to_hsv, mask);

    // remove noise
    auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, m_target_hp_min_height));
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel);

    // join parts of the bar
    kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(25, 1));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

    cv::imshow("", mask);

    // find external contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    return {};
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
