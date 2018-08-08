#pragma once

#include <optional>
#include <vector>

#include <opencv2/opencv.hpp>

#include "Eyes.h"
#include "Hands.h"

class Brain
{
public:
    Brain(::Eyes &eyes, ::Hands &hands) :
        m_eyes  {eyes},
        m_hands {hands}
    {}

    const std::vector<::Eyes::NPC> &NPCs() const        { return m_npcs; }
    const std::optional<::Eyes::Me> &Me() const         { return m_me; }
    const std::optional<::Eyes::Target> &Target() const { return m_target; }

    void Init();
    void Process(const cv::Mat &image);

private:
    ::Eyes &m_eyes;
    ::Hands &m_hands;

    std::vector<::Eyes::NPC> m_npcs;
    std::optional<::Eyes::Me> m_me;
    std::optional<::Eyes::Target> m_target;
};
