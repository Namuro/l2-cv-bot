#include "Brain.h"

#include <iostream>

void Brain::Init()
{
    std::cout << "Reset UI" << std::endl;
    m_hands.Delay(500);
    m_hands.ResetUI().ResetCamera().Send(500);
}

void Brain::Process()
{
    if (m_eyes.IsReady()) {
        m_npcs = m_eyes.DetectNPCs();
        m_me = m_eyes.DetectMe();
        m_target = m_eyes.DetectTarget();

        if (m_me.has_value()) {
            const auto me = m_me.value();

            if (me.hp < 70) {
                m_hands.RestoreHP().Send();
            }

            if (me.mp < 70) {
                m_hands.RestoreMP().Send();
            }

            if (me.cp < 70) {
                m_hands.RestoreCP().Send();
            }
        }
    }

    if (!m_hands.IsReady()) {
        return;
    }

    m_eyes.DetectMyBarsOnce();
    m_eyes.DetectTargetHPBarOnce();

    const auto target = m_target.value_or(::Eyes::Target{});

    if (m_state == State::Search) {
        if (target.hp > 0) {
            std::cout << "Attack target" << std::endl;
            m_npc_id = 0;
            m_hands.Spoil().ResetCamera().Send();
            m_state = State::Attack;
        } else {
            IgnoreNPC();

            std::cout << "Search target" << std::endl;
            const auto npc = UnselectedNPC();

            if (npc.has_value()) {
                std::cout << "Check target" << std::endl;
                m_npc_id = npc.value().CenterId();
                m_hands.MoveMouseToTarget({npc.value().center.x, npc.value().center.y}).Send(250);
                m_state = State::Check;
            } else {
                std::cout << "Look around" << std::endl;
                m_ignored_npcs.clear();
                m_hands.LookAround().Send(500);
            }
        }
    } else if (m_state == State::Check) {
        const auto npc = HoveredNPC();

        if (npc.has_value()) {
            m_hands.SelectTarget().Send(1000);
        }

        m_state = State::Search;
    } else if (m_state == State::Attack) {
        if (target.hp > 0) {
            m_hands.Attack().Send(200);
        } else if (!BRAIN_LOCKED(1000)) {
            const auto npc = SelectedNPC();

            if (npc.has_value()) {
                std::cout << "Go to target" << std::endl;
                // TODO: moving of the selected target can be detected
                m_hands.GoTo({npc.value().center.x, npc.value().center.y}).Send(5000);
            }

            m_state = State::PickUp;
        }
    } else if (m_state == State::PickUp) {
        std::cout << "Pick up loot" << std::endl;
        m_hands.Sweep().PickUp().CancelTarget().Send(500);
        m_state = State::Search;
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

std::vector<::Eyes::NPC> Brain::FilteredNPCs() const
{
    std::vector<::Eyes::NPC> npcs;

    for (const auto &npc : m_npcs) {
        if (m_ignored_npcs.find(npc.CenterId()) == m_ignored_npcs.end()) {
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

void Brain::IgnoreNPC()
{
    if (m_npc_id != 0) {
        std::cout << "Temporary ignore target" << std::endl;
        m_ignored_npcs.insert(m_npc_id);
        m_npc_id = 0;
    }
}
