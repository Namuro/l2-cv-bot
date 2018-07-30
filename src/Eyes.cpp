#include "Eyes.h"

void Eyes::Blink(const cv::Mat &rgb)
{
    cv::Mat hsv;
    cv::cvtColor(rgb, hsv, cv::COLOR_BGR2HSV);

    // detect target HP bar once
    if (!m_target_hp_bar.has_value()) {
        m_target_hp_bar = DetectTargetHPBar(hsv);
    }

    // detect my bars once
    if (!m_myself_bars.has_value()) {
        m_myself_bars = DetectMyselfBars(hsv);
    }

    // find out bar values (HP/MP/CP)
    m_myself = CalcMyselfValues(hsv);
    m_target = CalcTargetValues(hsv);

    // detect possible targets if there's no current target
    if (m_target.hp == 0) {
        m_possible_targets = DetectPossibleTargets(hsv);
    }
    else {
        m_possible_targets = {};
    }
}

void Eyes::Reset()
{
    m_myself_bars = {};
    m_target_hp_bar = {};
}

std::vector<Eyes::PossibleTarget> Eyes::DetectPossibleTargets(const cv::Mat &hsv) const
{
    // TL;DR: search for NPC names

    // extract white regions (NPC names)
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

    std::vector<PossibleTarget> targets;

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

        const auto name_image = white(rect);
        const auto threshold = cv::countNonZero(name_image) / name_image.total();

        if (threshold > m_target_color_threshold) {
            continue;
        }

        PossibleTarget target = {};
        target.rect = rect;
        target.center = cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2 + 15);
        target.id = Hash(name_image);
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
    const auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(25, m_target_hp_min_height));
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

std::optional<Eyes::MyselfBars> Eyes::DetectMyselfBars(const cv::Mat &hsv) const
{
    // TL;DR: search for HP bar, then detect CP bar above and MP bar below

    // exract HP bar color
    cv::Mat mask;
    cv::inRange(hsv, m_myself_hp_color_from_hsv, m_myself_hp_color_to_hsv, mask);

    const auto contours = FindBarContours(mask);

    // search for CP bar above and MP bar below
    for (const auto &contour : contours) {
        const auto rect = cv::boundingRect(contour);

        // check rect size & proportions
        if (rect.height < m_myself_bar_min_height || rect.height > m_myself_bar_max_height ||
            rect.width < m_myself_bar_min_width || rect.width > m_myself_bar_max_width
        ) {
            continue;
        }

        // search other bars near HP bar
        const auto bars_rect = rect + cv::Point(0, -rect.height * 2) + cv::Size(0, rect.height * 4);
        const auto bars = hsv(bars_rect);

        cv::Mat mp;
        cv::inRange(bars, m_myself_mp_color_from_hsv, m_myself_mp_color_to_hsv, mp);

        cv::Mat cp;
        cv::inRange(bars, m_myself_cp_color_from_hsv, m_myself_cp_color_to_hsv, cp);

        cv::Mat mp_cp;
        cv::bitwise_or(cp, mp, mp_cp);

        const auto bar_contours = FindBarContours(mp_cp);

        // no CP or MP bar found
        if (bar_contours.size() != 2) {
            continue;
        }

        MyselfBars myself_bars = {};
        myself_bars.hp_bar = rect;
        myself_bars.mp_bar = cv::boundingRect(bar_contours[0]) + bars_rect.tl();
        myself_bars.cp_bar = cv::boundingRect(bar_contours[1]) + bars_rect.tl();
        return myself_bars;
    }

    return {};
}

Eyes::Myself Eyes::CalcMyselfValues(const cv::Mat &hsv) const
{
    if (!m_myself_bars.has_value()) {
        return {};
    }

    auto hp_bar = hsv(m_myself_bars.value().hp_bar);
    auto mp_bar = hsv(m_myself_bars.value().mp_bar);
    auto cp_bar = hsv(m_myself_bars.value().cp_bar);

    cv::inRange(hp_bar, m_myself_hp_color_from_hsv, m_myself_hp_color_to_hsv, hp_bar);
    cv::inRange(mp_bar, m_myself_mp_color_from_hsv, m_myself_mp_color_to_hsv, mp_bar);
    cv::inRange(cp_bar, m_myself_cp_color_from_hsv, m_myself_cp_color_to_hsv, cp_bar);

    Myself myself = {};
    myself.hp = CalcBarPercentValue(hp_bar);
    myself.mp = CalcBarPercentValue(mp_bar);
    myself.cp = CalcBarPercentValue(cp_bar);
    return myself;
}

Eyes::Target Eyes::CalcTargetValues(const cv::Mat &hsv) const
{
    if (!m_target_hp_bar.has_value()) {
        return {};
    }

    auto hp_bar = hsv(m_target_hp_bar.value());
    cv::inRange(hp_bar, m_target_hp_color_from_hsv, m_target_hp_color_to_hsv, hp_bar);

    struct Target target = {};
    target.hp = CalcBarPercentValue(hp_bar);
    return target;
}

std::vector<std::vector<cv::Point>> Eyes::FindBarContours(const cv::Mat &mask) const
{
    // remove noise
    auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, m_myself_bar_min_height));
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

int Eyes::CalcBarPercentValue(const cv::Mat &bar)
{
    CV_Assert(bar.depth() == CV_8U);
    CV_Assert(bar.channels() == 1);

    const auto row = bar.ptr<uchar>(bar.rows / 2);
    auto col = bar.cols;

    // loop mid row until first white pixel
    for (; col-- > 0;) {
        if (row[col] == 255) {
            break;
        }
    }

    return col * 100 / bar.cols;
}

uint32_t Eyes::Hash(const cv::Mat &image)
{
    // djb2 hash
    uint32_t hash = 5381;
    const auto total = image.total();

    for (size_t i = 0; i < total; i++) {
        hash = ((hash << 5) + hash) ^ *(image.data + i);
    }

    return hash;
}
