#include "Gameplay/Player.h"

#include <algorithm>

#include "Core/EventBus.h"
#include "Core/Events.h"

Player::Player(EventBus& bus)
    : m_bus(bus), m_currentHp(0), m_maxHp(0), m_currentHeat(0), m_maxHeat(0) {}

void Player::resetForNewRun(int baseMaxHp, int baseMaxHeat) {
  m_maxHp = baseMaxHp;
  m_currentHp = baseMaxHp;

  m_maxHeat = baseMaxHeat;
  m_currentHeat = 0;

  m_artifacts.clear();
}

void Player::takeDamage(int amount) {
  if (amount <= 0 || m_currentHp <= 0) return;

  int oldHp = m_currentHp;
  m_currentHp = std::clamp(m_currentHp - amount, 0, m_maxHp);

  // Публикуем событие только при фактическом изменении
  if (m_currentHp != oldHp) {
    m_bus.publish(Evt_PlayerHPChanged{m_currentHp, m_maxHp});
  }

  // Если ХП упало до 0, генерируем поражение
  if (m_currentHp == 0) {
    m_bus.publish(Evt_CombatEnd{false});
  }
}

void Player::heal(int amount) {
  if (amount <= 0 || m_currentHp == m_maxHp) return;

  int oldHp = m_currentHp;
  m_currentHp = std::clamp(m_currentHp + amount, 0, m_maxHp);

  if (m_currentHp != oldHp) {
    m_bus.publish(Evt_PlayerHPChanged{m_currentHp, m_maxHp});
  }
}

void Player::addHeat(int amount) {
  if (amount <= 0) return;

  int oldHeat = m_currentHeat;
  // Жар может превышать m_maxHeat, поэтому не ограничиваем его сверху
  m_currentHeat = std::max(0, m_currentHeat + amount);

  if (m_currentHeat != oldHeat) {
    m_bus.publish(
        Evt_PlayerHeatChanged{m_currentHeat, m_maxHeat, isOverloaded()});
  }
}

void Player::reduceHeat(int amount) {
  if (amount <= 0 || m_currentHeat == 0) return;

  int oldHeat = m_currentHeat;
  m_currentHeat = std::max(0, m_currentHeat - amount);

  if (m_currentHeat != oldHeat) {
    m_bus.publish(
        Evt_PlayerHeatChanged{m_currentHeat, m_maxHeat, isOverloaded()});
  }
}

bool Player::isOverloaded() const {
  // Порог перегрузки - 85% от базового максимального Жара
  return m_currentHeat >= (m_maxHeat * 85 / 100);
}

void Player::processTurnEnd() {
  if (isOverloaded()) {
    // Урон от перегрузки (Магическое число 5 можно вынести в GameConstants)
    takeDamage(5);
  }
}

void Player::checkPostCombatMeltdown() {
  // Порог смерти после боя — Жар > 100%
  if (m_currentHeat > m_maxHeat) {
    takeDamage(m_currentHp);  // Отнимаем всё здоровье (мгновенная смерть)
  }
}

void Player::equipSkills(const std::vector<std::string>& skillIds) {
  m_equippedSkills = skillIds;
  // Гарантируем, что экипировано не больше 3 навыков
  if (m_equippedSkills.size() > 3) {
    m_equippedSkills.resize(3);
  }
}

const std::vector<std::string>& Player::getEquippedSkills() const {
  return m_equippedSkills;
}

void Player::addArtifact(const std::string& artifactId) {
  m_artifacts.push_back(artifactId);

  // Моментальные эффекты артефактов при получении
  if (artifactId == "armor_plate") {
    m_maxHp += 20;
    heal(20);  // heal() безопасно увеличит ХП с учетом нового предела и обновит
               // UI
  }
  // Остальные эффекты считывает CombatSystem через getArtifacts()
}

const std::vector<std::string>& Player::getArtifacts() const {
  return m_artifacts;
}

void Player::forceUpdateUI() {
  m_bus.publish(Evt_PlayerHPChanged{m_currentHp, m_maxHp});
  m_bus.publish(
      Evt_PlayerHeatChanged{m_currentHeat, m_maxHeat, isOverloaded()});
}
