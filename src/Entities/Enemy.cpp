#include "Entities/Enemy.h"

namespace solis {

Enemy::Enemy() {}

/**
  Наносит урон противнику.
 */
void Enemy::takeDamage(int damage) {
    m_currentHp -= damage;
    if (m_currentHp < 0) m_currentHp = 0;
}

/**
  Обновляет длительность статусных эффектов на враге.
 * Вызывается в конце каждого хода боя.
 */
void Enemy::processTurnEffects() {
    for (auto it = m_activeEffects.begin(); it != m_activeEffects.end();) {
        it->duration_turns--;
        if (it->duration_turns <= 0) {
            // Удаление эффекта по истечении времени действия
            it = m_activeEffects.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace solis
