#include "Entities/Player.h"
#include <algorithm>

namespace solis {

Player::Player() {}

void Player::takeDamage(int damage) {
    m_currentHp -= damage;
    if (m_currentHp < 0) m_currentHp = 0;
}

void Player::heal(int amount) {
    m_currentHp += amount;
    if (m_currentHp > m_maxHp) m_currentHp = m_maxHp;
}

void Player::addHeat(float amount) {
    m_heat += amount;
}

void Player::reduceHeat(float amount) {
    m_heat -= amount;
    if (m_heat < 0) m_heat = 0.0f;
}

} // namespace solis
