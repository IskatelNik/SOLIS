#include "Entities/BossEnemy.h"
#include <iostream>

namespace solis {

BossEnemy::BossEnemy() : Enemy() {}

/**
 * Проверяет и обновляет текущую фазу босса на основе остатка Воли (willpower).
 * При смене фазы меняются черты босса, его реплики и уровень агрессии.
 */
void BossEnemy::updatePhase() {
    if (m_phases.empty()) return;
    
    // Поиск фазы, соответствующей текущему значению willpower
    for (size_t i = 0; i < m_phases.size(); ++i) {
        if (m_currentWillpower == m_phases[i].willpower_left) {
            // Если фаза действительно изменилась — обновляем данные врага
            if (m_currentPhaseIndex != static_cast<int>(i)) {
                m_currentPhaseIndex = static_cast<int>(i);
                setTrait(m_phases[i].target_trait, m_phases[i].target_trait_name, m_phases[i].boss_replica);
                std::cout << "[BossEnemy] Phase changed to index " << i << std::endl;
            }
            break; 
        }
    }
}

} // namespace solis
