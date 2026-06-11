#include "Entities/Player.h"
#include <algorithm>
#include <random>

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
    return baseAmount * getHeatGainMultiplier();
}

float Player::getOverloadThreshold() const {
    float threshold = m_baseOverloadThreshold;
    for (const auto& art : m_artifacts) {
        if (art.modifier_type == "overload_threshold_flat") threshold += art.value;
    }
    return threshold;
}

int Player::getBaseDamageBonus() const {
    int bonus = m_baseDamageBonus;
    for (const auto& art : m_artifacts) {
        if (art.modifier_type == "base_damage_flat") bonus += (int)art.value;
    }
    return bonus;
}

float Player::getHeatGainMultiplier() const {
    float mult = m_baseHeatGainMultiplier;
    for (const auto& art : m_artifacts) {
        if (art.modifier_type == "heat_reduction_multiplier") mult *= art.value;
    }
    return mult;
}

float Player::getDefenseMultiplier() const {
    float mult = 1.0f;
    for (const auto& art : m_artifacts) {
        if (art.modifier_type == "defense_multiplier") mult *= art.value;
    }
    return mult;
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

std::string Player::removeRandomArtifact() {
    if (m_artifacts.empty()) return "";
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(m_artifacts.size()) - 1);
    
    int index = dis(gen);
    Artifact art = m_artifacts[index];
    
    // Reverse max_hp_flat logic
    if (art.modifier_type == "max_hp_flat") {
        m_maxHp -= static_cast<int>(art.value);
        if (m_currentHp > m_maxHp) m_currentHp = m_maxHp;
    }
    
    m_artifacts.erase(m_artifacts.begin() + index);
    return art.name;
}

} // namespace solis
