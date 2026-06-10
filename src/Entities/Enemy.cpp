#include "Entities/Enemy.h"

namespace solis {

Enemy::Enemy() {}

void Enemy::takeDamage(int damage) {
    m_currentHp -= damage;
    if (m_currentHp < 0) m_currentHp = 0;
}

void Enemy::processTurnEffects() {
    for (auto it = m_activeEffects.begin(); it != m_activeEffects.end();) {
        it->duration_turns--;
        if (it->duration_turns <= 0) {
            // MVP 5: Handle effect expiration (e.g., restore defense modifier if it was altered)
            // A more complex system would store the original value or calculate current from base + active effects.
            // For MVP 5, let's keep it simple: skills apply debuffs, and we'll handle removing them in CombatState or here.
            // For now just decrement and remove.
            it = m_activeEffects.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace solis
