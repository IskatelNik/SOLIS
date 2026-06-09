#include "Entities/BossEnemy.h"
#include <iostream>

namespace solis {

BossEnemy::BossEnemy() : Enemy() {}

void BossEnemy::updatePhase() {
    if (m_phases.empty()) return;
    
    // Find appropriate phase based on exact willpower left
    for (size_t i = 0; i < m_phases.size(); ++i) {
        if (m_currentWillpower == m_phases[i].willpower_left) {
            if (m_currentPhaseIndex != static_cast<int>(i)) {
                m_currentPhaseIndex = static_cast<int>(i);
                setTrait(m_phases[i].target_trait, m_phases[i].target_trait_name, m_phases[i].boss_replica);
                std::cout << "[BossEnemy] Phase changed to " << i << std::endl;
            }
            break; // Found the exact phase
        }
    }
}

} // namespace solis
