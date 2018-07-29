#pragma once

#include <opencv2/opencv.hpp>
#include <vector>

struct Target
{
    cv::Rect rect;
    cv::Point center;
    uint32_t id;
    float hp;
};

struct Me
{
    float hp;
    float mp;
    float cp;
};

class Eyes
{
    std::vector<Target> m_targets = {};
    Me m_me = {};

public:
    int m_target_min_height = 10;
    int m_target_max_height = 20;
    int m_target_min_width = 20;
    int m_target_max_width = 250;
    cv::Scalar m_target_color_from_hsv = cv::Scalar(0, 0, 240);
    cv::Scalar m_target_color_to_hsv = cv::Scalar(0, 0, 255);
    double m_target_color_threshold = 0.2;

    void Blink(const cv::Mat &rgb);
    std::vector<Target> Targets() const { return m_targets; }
    Me Me() const { return m_me; }

private:
    std::vector<Target> DetectTargets(const cv::Mat &hsv) const;
    struct Me DetectMyself(const cv::Mat &hsv) const;
    static uint32_t Hash(const cv::Mat &image);
};
