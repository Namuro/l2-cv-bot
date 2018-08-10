#pragma once

#include <vector>
#include <optional>

#include <opencv2/opencv.hpp>

class Eyes
{
public:
    struct MyBars { cv::Rect hp_bar, mp_bar, cp_bar; };
    struct Me { int hp, mp, cp; };
    struct Target { int hp; };

    struct NPC
    {
        uint32_t name_id;
        uint32_t center_id;
        cv::Point center;
        bool selected;
        cv::Rect rect;
    };

    // NPC detection
    int m_npc_name_min_height = 8;
    int m_npc_name_max_height = 16;
    int m_npc_name_min_width = 20;
    int m_npc_name_max_width = 250;
    cv::Scalar m_npc_name_color_from_hsv = {0, 0, 240};
    cv::Scalar m_npc_name_color_to_hsv = {0, 0, 255};
    double m_npc_name_color_threshold = 0.2;
    int m_npc_name_center_offset = 15;

    // selected target detection
    int m_target_circle_area_height = 25;
    int m_target_circle_area_width = 25;
    cv::Scalar m_target_blue_circle_color_from_hsv = {107, 255, 89};
    cv::Scalar m_target_blue_circle_color_to_hsv = {107, 255, 90};
    cv::Scalar m_target_red_circle_color_from_hsv = {0, 255, 122};
    cv::Scalar m_target_red_circle_color_to_hsv = {0, 255, 123};

    // my HP/MP/CP bars detection
    int m_my_bar_min_height = 10;
    int m_my_bar_max_height = 20;
    int m_my_bar_min_width = 140;
    int m_my_bar_max_width = 400;
    cv::Scalar m_my_hp_color_from_hsv = {2, 90, 120};
    cv::Scalar m_my_hp_color_to_hsv = {5, 220, 170};
    cv::Scalar m_my_mp_color_from_hsv = {105, 100, 130};
    cv::Scalar m_my_mp_color_to_hsv = {110, 255, 170};
    cv::Scalar m_my_cp_color_from_hsv = {16, 100, 120};
    cv::Scalar m_my_cp_color_to_hsv = {22, 255, 200};

    // target HP bar detection
    int m_target_hp_min_height = 3;
    int m_target_hp_max_height = 7;
    int m_target_hp_min_width = m_my_bar_min_width;
    int m_target_hp_max_width = m_my_bar_max_width;
    cv::Scalar m_target_hp_color_from_hsv = {0, 60, 80};
    cv::Scalar m_target_hp_color_to_hsv = {2, 220, 170};

    const std::optional<cv::Rect> &TargetHPBar() const { return m_target_hp_bar; }
    const std::optional<struct MyBars> &MyBars() const { return m_my_bars; }

    void Blink(const cv::Mat &rgb)  { cv::cvtColor(rgb, m_hsv, cv::COLOR_BGR2HSV); }
    void DetectMyBarsOnce()         { if (!m_my_bars.has_value()) m_my_bars = DetectMyBars(); }
    void DetectTargetHPBarOnce()    { if (!m_target_hp_bar.has_value()) m_target_hp_bar = DetectTargetHPBar(); }
    void Reset()                    { m_my_bars = {}; m_target_hp_bar = {}; }

    std::vector<NPC> DetectNPCs() const;
    std::optional<Me> DetectMe() const;
    std::optional<Target> DetectTarget() const;

private:
    cv::Mat m_hsv;
    std::optional<struct MyBars> m_my_bars;
    std::optional<cv::Rect> m_target_hp_bar;

    std::optional<struct MyBars> DetectMyBars() const;
    std::optional<cv::Rect> DetectTargetHPBar() const;
    std::vector<std::vector<cv::Point>> FindMyBarContours(const cv::Mat &mask) const;
    bool NPCSelected(const cv::Rect &rect) const;

    static bool RectInImage(const cv::Mat &image, const cv::Rect &rect)
        { return (rect & cv::Rect{0, 0, image.cols, image.rows}) == rect; }

    static int CalcBarPercentValue(const cv::Mat &bar, const cv::Scalar &from_color, const cv::Scalar &to_color);
    static uint32_t Hash(const cv::Mat &image);
};
