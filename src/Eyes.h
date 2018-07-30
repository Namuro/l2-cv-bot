#pragma once

#include <vector>
#include <tuple>
#include <optional>

#include <opencv2/opencv.hpp>

class Eyes
{
public:
    struct Target
    {
        int hp = 0;
    };

    struct Myself
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

    struct MyselfBars
    {
        cv::Rect hp_bar = {};
        cv::Rect mp_bar = {};
        cv::Rect cp_bar = {};
    };

private:
    std::vector<PossibleTarget> m_possible_targets;
    Myself m_myself;
    Target m_target;

    std::optional<MyselfBars> m_myself_bars;
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
    int m_myself_bar_min_height = 10;
    int m_myself_bar_max_height = 20;
    int m_myself_bar_min_width = 140;
    int m_myself_bar_max_width = 400;
    cv::Scalar m_myself_hp_color_from_hsv = cv::Scalar(2, 90, 120);
    cv::Scalar m_myself_hp_color_to_hsv = cv::Scalar(5, 220, 170);
    cv::Scalar m_myself_mp_color_from_hsv = cv::Scalar(105, 100, 130);
    cv::Scalar m_myself_mp_color_to_hsv = cv::Scalar(110, 255, 170);
    cv::Scalar m_myself_cp_color_from_hsv = cv::Scalar(16, 100, 120);
    cv::Scalar m_myself_cp_color_to_hsv = cv::Scalar(22, 255, 200);

    // target HP detection
    int m_target_hp_min_height = 3;
    int m_target_hp_max_height = 7;
    int m_target_hp_min_width = m_myself_bar_min_width;
    int m_target_hp_max_width = m_myself_bar_max_width;
    cv::Scalar m_target_hp_color_from_hsv = cv::Scalar(0, 60, 80);
    cv::Scalar m_target_hp_color_to_hsv = cv::Scalar(2, 220, 170);

    void Blink(const cv::Mat &rgb);
    void Reset();
    std::vector<PossibleTarget> GetPossibleTargets() const { return m_possible_targets; }
    Myself GetMyself() const { return m_myself; }
    Target GetTarget() const { return m_target; }
    std::optional<cv::Rect> GetTargetHPBar() const { return m_target_hp_bar; }
    std::optional<MyselfBars> GetMyselfBars() const { return m_myself_bars; }

private:
    std::vector<PossibleTarget> DetectPossibleTargets(const cv::Mat &hsv) const;
    std::optional<cv::Rect> DetectTargetHPBar(const cv::Mat &hsv) const;
    std::optional<MyselfBars> DetectMyselfBars(const cv::Mat &hsv) const;
    Myself CalcMyselfValues(const cv::Mat &hsv) const;
    Target CalcTargetValues(const cv::Mat &hsv) const;
    std::vector<std::vector<cv::Point>> FindBarContours(const cv::Mat &mask) const;
    static int CalcBarPercentValue(const cv::Mat &bar);
    static uint32_t Hash(const cv::Mat &image);
};
