#include "Brain.h"

#include <iostream>

#define LOCKED(ms) Locked(ms, __FILE__, __LINE__)

void Brain::Init()
{
    m_hands.Delay(500);
    m_hands.ResetUI();
    m_hands.ResetCamera();
    m_hands.Send(500);
}

void Brain::Process()
{
    m_npcs = m_eyes.DetectNPCs();
    m_far_npcs = m_eyes.DetectFarNPCs();
    m_me = m_eyes.DetectMe();
    m_target = m_eyes.DetectTarget();

    return;

    if (m_me.has_value()) {
        const auto me = m_me.value();

        if (me.hp < 70 && !LOCKED(1000)) {
            m_hands.RestoreHP();
            m_hands.Send();
        }

        if (me.mp < 70 && !LOCKED(1000)) {
            m_hands.RestoreMP();
            m_hands.Send();
        }

        if (me.cp < 70 && !LOCKED(1000)) {
            m_hands.RestoreCP();
            m_hands.Send();
        }
    }

    const auto target = m_target.value_or(::Eyes::Target{});

    if (target.hp > 0) {
        m_state = State::Attack;
    }

    if (!m_hands.IsReady()) {
        return;
    }

    if (m_state == State::NextTarget) {
        m_hands.NextTarget();
        m_hands.Send(500);
        m_state = State::NearSearch;
    } else if (m_state == State::NearSearch) {
        const auto npc = UnselectedNPC();

        if (npc.has_value()) {
            IgnoreNPC(npc.value().Id());
            m_hands.MoveMouseTo({npc.value().center.x, npc.value().center.y});
            m_hands.Send(500);
            m_previous_state = m_state;
            m_state = State::Check;
        } else if (m_search_attempt < m_search_attempts) {
            ++m_search_attempt;
            m_hands.LookAround();
            m_hands.NextTarget();
            m_hands.Send(500);
            ClearIgnoredNPCs();
        } else {
            m_state = State::FarSearch;
            m_search_attempt = 0;
        }
    } else if (m_state == State::FarSearch) {
        if (LOCKED(1000)) {
            return;
        }

        const auto npc = FarNPC();

        if (npc.has_value()) {
            IgnoreNPC(npc.value().Id());
            m_hands.MoveMouseTo({npc.value().center.x, npc.value().center.y});
            m_hands.Send(500);
            m_previous_state = m_state;
            m_state = State::Check;
        } else if (m_search_attempt < m_search_attempts) {
            ++m_search_attempt;
            m_hands.LookAround();
            m_hands.NextTarget();
            m_hands.Send(500);
            ClearIgnoredNPCs();
        } else {
            m_state = State::NextTarget;
            m_search_attempt = 0;
        }
    } else if (m_state == State::Check) {
        const auto npc = HoveredNPC();

        if (npc.has_value()) {
            m_hands.SelectTarget();
            m_hands.Send(500);
        }

        m_state = m_previous_state;
    } else if (m_state == State::Attack) {
        if (target.hp > 0) {
            if (m_first_attack) {
                m_first_attack = false;
                UnignoreNPC();
                m_hands.Spoil();
                m_hands.Attack();
                m_hands.Delay(500);
                m_hands.ResetCamera();
                m_hands.Send();
            } else {
                m_hands.Attack();
                m_hands.Send(250);
            }
        } else if (!LOCKED(1000)) {
            m_first_attack = true;
            const auto npc = SelectedNPC();

            if (npc.has_value()) {
                m_hands.GoTo({npc.value().center.x, npc.value().center.y});
                m_hands.Send(4000); // TODO: moving of the selected target can be detected
            }

            m_state = State::PickUp;
        }
    } else if (m_state == State::PickUp) {
        m_hands.Sweep();
        m_hands.Delay(500);
        m_hands.PickUp();
        m_hands.CancelTarget();
        m_hands.Send();
        m_state = State::NextTarget;
    }
}

std::optional<::Eyes::NPC> Brain::HoveredNPC() const
{
    const auto npcs = FilteredNPCs();

    for (const auto &npc : npcs) {
        if (npc.Hovered()) {
            return npc;
        }
    }

    return {};
}

std::optional<::Eyes::NPC> Brain::SelectedNPC() const
{
    const auto npcs = FilteredNPCs();

    for (const auto &npc : npcs) {
        if (npc.Selected()) {
            return npc;
        }
    }

    return {};
}

std::optional<::Eyes::NPC> Brain::UnselectedNPC() const
{
    const auto npcs = FilteredNPCs();

    for (const auto &npc : npcs) {
        if (!npc.Selected()) {
            return npc;
        }
    }

    return {};
}

std::optional<::Eyes::FarNPC> Brain::FarNPC() const
{
    for (const auto &npc : m_far_npcs) {
        if (m_ignored_npc_ids.find(npc.Id()) == m_ignored_npc_ids.end()) {
            return npc;
        }
    }

    return {};
}

std::vector<::Eyes::NPC> Brain::FilteredNPCs() const
{
    std::vector<::Eyes::NPC> npcs;

    for (const auto &npc : m_npcs) {
        if (m_ignored_npc_ids.find(npc.Id()) == m_ignored_npc_ids.end()) {
            npcs.push_back(npc);
        }
    }

    return npcs;
}

bool Brain::Locked(int ms, const std::string &file, int line)
{
    const auto key = file + ":" + std::to_string(line);
    const auto lock = m_locks.find(key);
    const auto now = std::chrono::steady_clock::now();

    if (lock == m_locks.end()) {
        m_locks[key] = now + std::chrono::milliseconds(ms);
        return true;
    } else if (lock->second < now) {
        m_locks.erase(lock);
        return false;
    }

    return true;
}
