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

float Player::calculateHeatGain(float baseAmount) const {
    float finalAmount = baseAmount;
    for (const auto& artifact : m_artifacts) {
        if (artifact.modifier_type == "heat_gain_mult") {
            finalAmount *= artifact.value;
        }
    }
    return finalAmount;
}

void Player::processTurnEffects() {
    for (auto it = m_activeEffects.begin(); it != m_activeEffects.end();) {
        it->duration_turns--;
        if (it->duration_turns <= 0) {
            it = m_activeEffects.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace solis
