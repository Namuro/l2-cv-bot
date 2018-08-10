#pragma once

#include <optional>
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <optional>

#include "Eyes.h"
#include "Hands.h"

#define BRAIN_LOCKED(ms) Locked(ms, __FILE__, __LINE__)

class Brain
{
public:
    Brain(::Eyes &eyes, ::Hands &hands) :
        m_state {State::Search},
        m_eyes  {eyes},
        m_hands {hands},
        m_npc_id{0}
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
        Check  = Search << 1,
        Attack = Search << 2,
        PickUp = Search << 3
    };

    std::map<std::string, std::chrono::time_point<std::chrono::steady_clock>> m_locks;
    State m_state;
    ::Eyes &m_eyes;
    ::Hands &m_hands;
    uint32_t m_npc_id;
    std::set<uint32_t> m_ignored_npcs;

    std::vector<::Eyes::NPC> m_npcs;
    std::optional<::Eyes::Me> m_me;
    std::optional<::Eyes::Target> m_target;

    std::optional<::Eyes::NPC> HoveredNPC() const;
    std::optional<::Eyes::NPC> SelectedNPC() const;
    std::optional<::Eyes::NPC> UnselectedNPC() const;
    std::vector<::Eyes::NPC> FilteredNPCs() const;
    void IgnoreNPC();
    bool Locked(int ms, const std::string &file, int line);
};
