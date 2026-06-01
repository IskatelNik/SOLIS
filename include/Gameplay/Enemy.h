#ifndef SOLIS_ENEMY_H
#define SOLIS_ENEMY_H

#include <string>

#include "Core/Events.h"

// Подключаем твой файл с определениями данных (путь может немного отличаться в
// зависимости от папок)
#include "Data/DataDefs.h"

class EventBus;

class Enemy {
 public:
  // Инициализация (использует EnemyDef из DataDefs.hpp)
  Enemy(EventBus& bus, const EnemyDef& def);

  // Базовые геттеры
  const std::string& getId() const;
  const std::string& getName() const;
  const std::string& getHiddenTrait() const;

  // Управление здоровьем (Физический бой)
  void takeDamage(int amount);
  bool isDead() const;

  // Управление Волей и Диалогами (Социальный бой)
  void reduceWillpower(int amount);
  bool isSociallyDefeated() const;
  void blockDialogue(int turns);
  bool isDialogueBlocked() const;
  void tickTurn();

  // Баффы и Дебаффы (Манипуляция Энергией)
  void applyDamageModifier(float percent);
  void applyArmorModifier(float percent);
  int calculateAttackDamage() const;

  // Синхронизация с UI
  void forceUpdateUI();

 private:
  EventBus& m_bus;

  std::string m_id;
  std::string m_name;
  std::string m_hiddenTrait;

  int m_currentHp;
  int m_maxHp;
  int m_currentWill;
  int m_maxWill;
  int m_baseDamage;

  float m_damageMultiplier;
  float m_armorMultiplier;
  int m_dialogueBlockTurns;
};

#endif  // SOLIS_ENEMY_H
