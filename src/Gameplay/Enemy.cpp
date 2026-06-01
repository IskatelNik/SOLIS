#include "Gameplay/Enemy.h"

#include <algorithm>
#include <cmath>

#include "Core/EventBus.h"  // Предполагается, что здесь объявлен метод publish()

Enemy::Enemy(EventBus& bus, const EnemyDef& def)
    : m_bus(bus),
      m_id(def.id),
      m_name(def.name),
      m_hiddenTrait(def.hiddenTrait),
      m_currentHp(def.maxHp),
      m_maxHp(def.maxHp),
      m_currentWill(def.maxWill),
      m_maxWill(def.maxWill),
      m_baseDamage(def.baseDamage),
      m_damageMultiplier(1.0f),
      m_armorMultiplier(1.0f),
      m_dialogueBlockTurns(0) {}

const std::string& Enemy::getId() const { return m_id; }
const std::string& Enemy::getName() const { return m_name; }
const std::string& Enemy::getHiddenTrait() const { return m_hiddenTrait; }

void Enemy::takeDamage(int amount) {
  if (amount <= 0) return;  // Защита от отрицательного урона

  // Применяем модификатор брони (например, 1.5f означает, что враг получает на
  // 50% больше урона)
  int actualDamage = static_cast<int>(std::round(amount * m_armorMultiplier));

  m_currentHp = std::max(0, m_currentHp - actualDamage);

  // Оповещаем UI
  m_bus.publish(Evt_EnemyHPChanged{m_currentHp, m_maxHp});
}

bool Enemy::isDead() const { return m_currentHp <= 0; }

void Enemy::reduceWillpower(int amount) {
  if (amount <= 0) return;

  m_currentWill = std::max(0, m_currentWill - amount);

  // Оповещаем UI
  m_bus.publish(Evt_EnemyWillpowerChanged{m_currentWill, m_maxWill});
}

bool Enemy::isSociallyDefeated() const { return m_currentWill <= 0; }

void Enemy::blockDialogue(int turns) {
  // Если диалог уже заблокирован, можно либо добавлять, либо устанавливать
  // максимум. Остановимся на простом переназначении/увеличении.
  m_dialogueBlockTurns = std::max(m_dialogueBlockTurns, turns);
}

bool Enemy::isDialogueBlocked() const { return m_dialogueBlockTurns > 0; }

void Enemy::tickTurn() {
  m_dialogueBlockTurns = std::max(0, m_dialogueBlockTurns - 1);
}

void Enemy::applyDamageModifier(float percent) {
  // Если percent = 0.5f, то множитель станет 1.5f (+50% урона)
  m_damageMultiplier += percent;
  // Защита от ухода в отрицательные значения (урон не может исцелять)
  m_damageMultiplier = std::max(0.0f, m_damageMultiplier);
}

void Enemy::applyArmorModifier(float percent) {
  // Входящий урон увеличивается или уменьшается (дебафф брони увеличивает
  // множитель получаемого урона)
  m_armorMultiplier += percent;
  m_armorMultiplier = std::max(0.0f, m_armorMultiplier);
}

int Enemy::calculateAttackDamage() const {
  return static_cast<int>(std::round(m_baseDamage * m_damageMultiplier));
}

void Enemy::forceUpdateUI() {
  // Вызывается RunManager-ом/CombatSystem при начале боя, чтобы UI сразу
  // обновил бары
  m_bus.publish(Evt_EnemyHPChanged{m_currentHp, m_maxHp});
  m_bus.publish(Evt_EnemyWillpowerChanged{m_currentWill, m_maxWill});
}
