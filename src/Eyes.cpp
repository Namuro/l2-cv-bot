#include "Eyes.h"

void Eyes::Blink(const cv::Mat &rgb)
{
    cv::cvtColor(rgb, m_hsv, cv::COLOR_BGR2HSV);

    // detect my bars once
    if (!m_my_bars.has_value()) {
        m_my_bars = DetectMyBars(m_hsv);
    }

    // detect target HP bar once
    if (!m_target_hp_bar.has_value()) {
        m_target_hp_bar = DetectTargetHPBar(m_hsv);
    }
}

void Eyes::Reset()
{
    m_my_bars = {};
    m_target_hp_bar = {};
}

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
        npc.id = Hash(target_image);
        npcs.push_back(npc);
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

std::optional<Eyes::Target> Eyes::DetectTarget(bool position) const
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

    if (!position) {
        return target;
    }

    // extract red regions with circles
    cv::Mat mask;
    cv::GaussianBlur(m_hsv, mask, {7, 7}, 0);
    cv::inRange(mask, m_target_circle_color_from_hsv, m_target_circle_color_to_hsv, mask);

    // remove noise
    const auto kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, {7, 7});
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Rect> circles;

    for (const auto &contour : contours) {
        const auto rect = cv::boundingRect(contour);

        if (rect.height < m_target_circle_min_height || rect.height > m_target_circle_max_height ||
            rect.width < m_target_circle_min_width || rect.width > m_target_circle_max_width
        ) {
            continue;
        }

        circles.push_back(rect);
    }

    // compare each element to find circles pair
    for (const auto &circle1 : circles) {
        for (const auto &circle2 : circles) {
            if (circle1.y != circle2.y || circle1.size() != circle2.size() ||
                std::abs(circle1.x - circle2.x) < m_npc_name_min_width + 5 ||
                std::abs(circle1.x - circle2.x) > m_npc_name_max_width + 5
            ) {
                continue;
            }

            const auto lcircle = circle1.x < circle2.x ? circle1 : circle2;
            const auto rcircle = circle1.x > circle2.x ? circle1 : circle2;

            target.left_circle = lcircle;
            target.right_circle = rcircle;

            target.center = {
                lcircle.x + (rcircle.x + rcircle.width - lcircle.x) / 2,
                lcircle.y + lcircle.height + m_target_center_offset
            };

            break;
        }
    }

    return target;
}

std::optional<struct Eyes::MyBars> Eyes::DetectMyBars(const cv::Mat &hsv) const
{
    // extract red regions with red HP bar
    cv::Mat mask;
    cv::inRange(hsv, m_my_hp_color_from_hsv, m_my_hp_color_to_hsv, mask);

    const auto contours = FindMyBarContours(mask);

    // search for CP bar above and MP bar below
    for (const auto &contour : contours) {
        const auto rect = cv::boundingRect(contour);

        if (rect.height < m_my_bar_min_height || rect.height > m_my_bar_max_height ||
            rect.width < m_my_bar_min_width || rect.width > m_my_bar_max_width
        ) {
            continue;
        }

        const auto bars_rect = rect + cv::Point(0, -rect.height * 2) + cv::Size(0, rect.height * 4);
        const auto bars = hsv(bars_rect);

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

std::optional<cv::Rect> Eyes::DetectTargetHPBar(const cv::Mat &hsv) const
{
    // extract red regions with red HP bar
    cv::Mat mask;
    cv::inRange(hsv, m_target_hp_color_from_hsv, m_target_hp_color_to_hsv, mask);
    
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

    for (std::size_t i = 0; i < total; i++) {
        hash = ((hash << 5) + hash) ^ *(image.data + i);
    }

    return hash;
}
