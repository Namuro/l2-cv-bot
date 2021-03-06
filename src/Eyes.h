#pragma once

#include <vector>
#include <optional>
#include <array>

#include <opencv2/opencv.hpp>

class Eyes
{
public:
    struct MyBars { cv::Rect hp_bar, mp_bar, cp_bar; };
    struct Me { int hp, mp, cp; };
    struct Target { int hp; };

    struct TrackableNPC {
        cv::Point center;
        std::uint32_t tracking_id;
        virtual std::uint32_t Id() const = 0;
    };

    struct NPC : TrackableNPC
    {
        enum class State { Default = 0, Hovered = 1, Selected = 2 };

        State state;
        std::uint32_t name_id;
        cv::Rect rect;

        std::uint32_t Id() const override   { return tracking_id > 0 ? tracking_id : CenterId(); }
        std::uint32_t CenterId() const      { return center.x << 16 | center.y; }
        bool Selected() const               { return state == State::Selected; }
        bool Hovered() const                { return state == State::Hovered; }
    };

    struct FarNPC : TrackableNPC
    {
        cv::Rect rect;
        std::uint32_t Id() const override { return tracking_id; }
    };

    int m_blind_spot_radius     = 100;
    int m_npc_tracking_distance = 30;

    // NPC detection
    int m_npc_name_min_height               = 8;
    int m_npc_name_max_height               = 16;
    int m_npc_name_min_width                = 20;
    int m_npc_name_max_width                = 250;
    cv::Scalar m_npc_name_color_from_hsv    = {0, 0, 240};
    cv::Scalar m_npc_name_color_to_hsv      = {0, 0, 255};
    double m_npc_name_color_threshold       = 0.2;
    int m_npc_name_center_offset            = 17;

    // far NPC detection
    int m_far_npc_min_height    = 20;
    int m_far_npc_max_height    = 200;
    int m_far_npc_min_width     = 20;
    int m_far_npc_max_width     = 200;
    int m_far_npc_limit         = 10;

    // selected target detection
    int m_target_circle_area_height             = 25;
    int m_target_circle_area_width              = 25;
    cv::Scalar m_target_gray_circle_color_bgr   = {57, 60, 66, 255};
    cv::Scalar m_target_blue_circle_color_bgr   = {107, 48, 0, 255};
    cv::Scalar m_target_red_circle_color_bgr    = {0, 4, 132, 255};

    // my HP/MP/CP bars detection
    int m_my_bar_min_height             = 10;
    int m_my_bar_max_height             = 20;
    int m_my_bar_min_width              = 140;
    int m_my_bar_max_width              = 400;
    cv::Scalar m_my_hp_color_from_hsv   = {2, 90, 120};
    cv::Scalar m_my_hp_color_to_hsv     = {5, 220, 170};
    cv::Scalar m_my_mp_color_from_hsv   = {105, 100, 130};
    cv::Scalar m_my_mp_color_to_hsv     = {110, 255, 170};
    cv::Scalar m_my_cp_color_from_hsv   = {16, 100, 120};
    cv::Scalar m_my_cp_color_to_hsv     = {22, 255, 200};

    // target HP bar detection
    int m_target_hp_min_height              = 3;
    int m_target_hp_max_height              = 7;
    int m_target_hp_min_width               = m_my_bar_min_width;
    int m_target_hp_max_width               = m_my_bar_max_width;
    cv::Scalar m_target_hp_color_from_hsv   = {0, 100, 100};
    cv::Scalar m_target_hp_color_to_hsv     = {2, 220, 140};

    Eyes() :
        m_hsv_frames{},
        m_frame     {0},
        m_diffs     {}
    {}

    const std::optional<cv::Rect> &TargetHPBar() const { return m_target_hp_bar; }
    const std::optional<struct MyBars> &MyBars() const { return m_my_bars; }

    void Open(const cv::Mat &bgr);
    void Close()    { m_hsv_frames[m_frame++ % m_hsv_frames.size()] = m_hsv.clone(); }
    void Reset()    { m_my_bars = {}; m_target_hp_bar = {}; }

    std::vector<NPC> DetectNPCs();
    std::vector<FarNPC> DetectFarNPCs();
    std::optional<Me> DetectMe();
    std::optional<Target> DetectTarget();

private:
    cv::Mat m_bgr;
    cv::Mat m_hsv;
    std::array<cv::Mat, 5> m_hsv_frames; // previous frames buffer
    std::array<cv::Mat, 5>::size_type m_frame; // current frame index

    std::optional<struct MyBars> m_my_bars;
    std::optional<cv::Rect> m_target_hp_bar;
    std::array<cv::Mat, 15> m_diffs; // used for far NPCs detection
    std::vector<NPC> m_npcs; // previously detected NPCs
    std::vector<FarNPC> m_far_npcs; // previously detected far NPCs

    std::optional<struct MyBars> DetectMyBars() const;
    std::optional<cv::Rect> DetectTargetHPBar() const;
    std::vector<std::vector<cv::Point>> FindMyBarContours(const cv::Mat &mask) const;
    NPC::State DetectNPCState(const cv::Rect &rect) const;

    template<typename T>
    void CalculateTrackingIds(std::vector<T> &npcs) const;

    static bool IsRectInImage(const cv::Mat &image, const cv::Rect &rect)
        { return (rect & cv::Rect{0, 0, image.cols, image.rows}) == rect; }

    static int CalcBarPercentValue(
        const cv::Mat &bar,
        const cv::Scalar &from_color,
        const cv::Scalar &to_color,
        bool whole_bar = false
    );

    static std::uint32_t Hash(const cv::Mat &image);
};
