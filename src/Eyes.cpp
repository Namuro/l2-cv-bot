#include "Eyes.h"

void Eyes::Blink(const cv::Mat &rgb)
{
    cv::Mat hsv;
    cv::cvtColor(rgb, hsv, cv::COLOR_BGR2HSV);

    m_targets = DetectTargets(hsv);
}

std::vector<Target> Eyes::DetectTargets(const cv::Mat &hsv) const
{
    // extract white regions (target names)
    cv::Mat white;
    cv::inRange(hsv, m_target_color_from_hsv, m_target_color_to_hsv, white);

    // increase white regions size
    cv::Mat mask;
    auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(white, mask, kernel);

    // join words in target names
    kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 5));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

    // remove noise
    cv::erode(mask, mask, kernel);
    cv::dilate(mask, mask, kernel, cv::Point(-1, -1), 2);

    // find external contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<Target> targets;

    for (auto &contour : contours) {
        auto rect = cv::boundingRect(contour);

        if (rect.height >= m_target_min_height && rect.height <= m_target_max_height &&
            rect.width >= m_target_min_width && rect.width <= m_target_max_width &&
            rect.width > rect.height * 1.25
        ) {
            auto target_image = white(rect);
            auto threshold = cv::countNonZero(target_image) / target_image.total();

            if (threshold <= m_target_color_threshold) {
                Target target = {};
                target.rect = rect;
                target.center = cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2 + 20);
                target.id = Hash(target_image);
                targets.push_back(target);
            }
        }
    }

    return targets;
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
