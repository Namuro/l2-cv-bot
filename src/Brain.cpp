#include "Brain.h"

void Brain::Init()
{
    m_hands.ResetUI();
    m_hands.Send();
}

void Brain::Process(const cv::Mat &image)
{
    if (!m_hands.Ready()) {
        return;
    }

    m_eyes.Blink(image);

    m_npcs = m_eyes.DetectNPCs();
    m_me = m_eyes.DetectMe();
    m_target = m_eyes.DetectTarget(true);
}
