#include "Entities/Player.h"
#include <algorithm>
#include <random>

namespace solis {

Player::Player() {}

/**
 * @brief Наносит урон игроку, уменьшая текущее HP.
 */
void Player::takeDamage(int damage) {
    m_currentHp -= damage;
    if (m_currentHp < 0) m_currentHp = 0;
}

/**
 * @brief Восстанавливает здоровье игрока, не превышая максимум.
 */
void Player::heal(int amount) {
    m_currentHp += amount;
    if (m_currentHp > m_maxHp) m_currentHp = m_maxHp;
}

/**
 * @brief Увеличивает текущий уровень Жара.
 */
void Player::addHeat(float amount) {
    m_heat += amount;
}

/**
 * @brief Снижает текущий уровень Жара.
 */
void Player::reduceHeat(float amount) {
    m_heat -= amount;
    if (m_heat < 0) m_heat = 0.0f;
}

/**
 * @brief Рассчитывает итоговое накопление Жара с учетом всех множителей (улучшения + артефакты).
 */
float Player::calculateHeatGain(float baseAmount) const {
    return baseAmount * getHeatGainMultiplier();
}

/**
 * @brief Возвращает текущий порог перегрузки (базовый + бонусы от артефактов).
 */
float Player::getOverloadThreshold() const {
    float threshold = m_baseOverloadThreshold;
    for (const auto& art : m_artifacts) {
        if (art.modifier_type == "overload_threshold_flat") threshold += art.value;
    }
    return threshold;
}

/**
 * @brief Возвращает суммарный бонус к базовому урону от всех источников.
 */
int Player::getBaseDamageBonus() const {
    int bonus = m_baseDamageBonus;
    for (const auto& art : m_artifacts) {
        if (art.modifier_type == "base_damage_flat") bonus += (int)art.value;
    }
    return bonus;
}

/**
 * @brief Возвращает итоговый множитель накопления Жара.
 */
float Player::getHeatGainMultiplier() const {
    float mult = m_baseHeatGainMultiplier;
    for (const auto& art : m_artifacts) {
        if (art.modifier_type == "heat_reduction_multiplier") mult *= art.value;
    }
    return mult;
}

/**
 * @brief Возвращает итоговый множитель защиты (получаемого урона).
 */
float Player::getDefenseMultiplier() const {
    float mult = 1.0f;
    for (const auto& art : m_artifacts) {
        if (art.modifier_type == "defense_multiplier") mult *= art.value;
    }
    return mult;
}

/**
 * @brief Обновляет длительность активных статусных эффектов (баффы/дебаффы).
 */
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

/**
 * @brief Случайным образом удаляет один артефакт из инвентаря (механика Тьмы на 3-м уровне).
 * @return Название удаленного артефакта.
 */
std::string Player::removeRandomArtifact() {
    if (m_artifacts.empty()) return "";
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, static_cast<int>(m_artifacts.size()) - 1);
    
    int index = dis(gen);
    Artifact art = m_artifacts[index];
    
    // Если артефакт давал бонус к макс. HP — корректируем текущие показатели
    if (art.modifier_type == "max_hp_flat") {
        m_maxHp -= static_cast<int>(art.value);
        if (m_currentHp > m_maxHp) m_currentHp = m_maxHp;
    }
    
    m_artifacts.erase(m_artifacts.begin() + index);
    return art.name;
}

} // namespace solis
