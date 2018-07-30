#pragma once

#include <opencv2/opencv.hpp>
#include <vector>
#include <tuple>
#include <optional>

struct Target
{
    int hp = 0;
};

struct Me
{
    int hp = 0;
    int mp = 0;
    int cp = 0;
};

struct PossibleTarget
{
    uint32_t id = 0;
    cv::Point center = {};
    cv::Rect rect = {};
};

struct MyBars
{
    cv::Rect hp_bar = {};
    cv::Rect mp_bar = {};
    cv::Rect cp_bar = {};
};

class Eyes
{
    std::vector<PossibleTarget> m_possible_targets;
    Me m_me = {};
    Target m_target = {};

    std::optional<MyBars> m_my_bars;
    std::optional<cv::Rect> m_target_hp_bar;

public:
    // possible targets detection
    int m_target_min_height = 10;
    int m_target_max_height = 20;
    int m_target_min_width = 20;
    int m_target_max_width = 250;
    cv::Scalar m_target_color_from_hsv = cv::Scalar(0, 0, 240);
    cv::Scalar m_target_color_to_hsv = cv::Scalar(0, 0, 255);
    double m_target_color_threshold = 0.2;

    // my HP/MP/CP detection
    int m_my_bar_min_height = 10;
    int m_my_bar_max_height = 20;
    int m_my_bar_min_width = 140;
    int m_my_bar_max_width = 400;
    cv::Scalar m_my_hp_color_from_hsv = cv::Scalar(2, 90, 120);
    cv::Scalar m_my_hp_color_to_hsv = cv::Scalar(5, 220, 170);
    cv::Scalar m_my_mp_color_from_hsv = cv::Scalar(105, 100, 130);
    cv::Scalar m_my_mp_color_to_hsv = cv::Scalar(110, 255, 170);
    cv::Scalar m_my_cp_color_from_hsv = cv::Scalar(16, 100, 120);
    cv::Scalar m_my_cp_color_to_hsv = cv::Scalar(22, 255, 200);

    // target HP detection
    int m_target_hp_min_height = 3;
    int m_target_hp_max_height = 7;
    int m_target_hp_min_width = m_my_bar_min_width;
    int m_target_hp_max_width = m_my_bar_max_width;
    cv::Scalar m_target_hp_color_from_hsv = cv::Scalar(0, 60, 80);
    cv::Scalar m_target_hp_color_to_hsv = cv::Scalar(2, 220, 170);

    void Blink(const cv::Mat &rgb);
    void Reset();
    std::vector<PossibleTarget> PossibleTargets() const { return m_possible_targets; }
    Me Me() const { return m_me; }
    Target Target() const { return m_target; }
    std::optional<cv::Rect> TargetHPBar() const { return m_target_hp_bar; }
    std::optional<MyBars> MyBars() const { return m_my_bars; }

private:
    std::vector<PossibleTarget> DetectPossibleTargets(const cv::Mat &hsv) const;
    std::optional<cv::Rect> DetectTargetHPBar(const cv::Mat &hsv) const;
    std::optional<struct MyBars> DetectMyBars(const cv::Mat &hsv) const;
    struct Me MyValues(const cv::Mat &hsv) const;
    struct Target TargetValues(const cv::Mat &hsv) const;
    std::vector<std::vector<cv::Point>> BarContours(const cv::Mat &mask) const;
    static int BarValue(const cv::Mat &bar);
    static uint32_t Hash(const cv::Mat &image);
};
