#pragma once

#include <optional>
#include <vector>

#include "Eyes.h"
#include "Hands.h"

class Brain
{
public:
    Brain(::Eyes &eyes, ::Hands &hands) :
        m_state {State::Search},
        m_eyes  {eyes},
        m_hands {hands}
    {}

    const std::vector<::Eyes::NPC> &NPCs() const        { return m_npcs; }
    const std::optional<::Eyes::Me> &Me() const         { return m_me; }
    const std::optional<::Eyes::Target> &Target() const { return m_target; }

    void Init();
    void Process();

private:
    enum class State
    {
        Search = 0x1,
        Attack = Search << 1,
        PickUp = Search << 2
    };

    State m_state;
    ::Eyes &m_eyes;
    ::Hands &m_hands;
    //uint32_t m_npc_id;
    //std::set<uint32_t> m_ignored_npcs;

    std::vector<::Eyes::NPC> m_npcs;
    std::optional<::Eyes::Me> m_me;
    std::optional<::Eyes::Target> m_target;
};
