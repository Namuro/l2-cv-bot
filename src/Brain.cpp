#include "Brain.h"

#include <iostream>

void Brain::Init()
{
    m_hands.Delay(500);
    m_hands.ResetUI().ResetCamera().Send(500);
}

void Brain::Process()
{
    if (!m_hands.Ready()) {
        return;
    }

    m_eyes.DetectMyBarsOnce();
    m_eyes.DetectTargetHPBarOnce();

    m_npcs = m_eyes.DetectNPCs();
    m_me = m_eyes.DetectMe();
    m_target = m_eyes.DetectTarget();

    const auto target = m_target.value_or(::Eyes::Target{});

    if (m_state == State::Search) {
        if (target.hp > 0) {
            std::cout << "Attack target" << std::endl;
            m_hands.ResetCamera().Send();
            m_state = State::Attack;
        } else {
            std::cout << "Search new targets" << std::endl;
            bool found = false;

            if (!m_npcs.empty()) {
                std::cout << "Select target" << std::endl;

                for (const auto &npc : m_npcs) {
                    if (!npc.selected) {
                        found = true;
                        m_hands.SelectTarget({npc.center.x, npc.center.y}).Send(500);
                        break;
                    }
                }
            }

            if (!found) {
                std::cout << "Look around" << std::endl;
                m_hands.LookAround().Send(500);
            }
        }
    } else if (m_state == State::Attack) {
        if (target.hp > 0) {
            m_hands.Attack().Send();
        } else {
            std::cout << "Go to target" << std::endl;
            m_state = State::PickUp;
        }
    } else if (m_state == State::PickUp) {
        std::cout << "Pickup" << std::endl;
        m_state = State::Search;
    }
}
