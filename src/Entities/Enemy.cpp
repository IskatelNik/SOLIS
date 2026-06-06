#include "Entities/Enemy.h"

namespace solis {

Enemy::Enemy() {}

void Enemy::takeDamage(int damage) {
    m_currentHp -= damage;
    if (m_currentHp < 0) m_currentHp = 0;
}

} // namespace solis
