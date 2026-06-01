#ifndef SOLIS_COMBAT_SYSTEM_H
#define SOLIS_COMBAT_SYSTEM_H

#include <memory>
#include <string>
#include <vector>

#include "Core/Events.h"

// Предварительные объявления (Forward declarations)
class EventBus;
class Player;
class DataLoader;
class SaveManager;
class Enemy;

class CombatSystem {
 public:
  CombatSystem(EventBus& bus, Player& player, DataLoader& dataLoader,
               SaveManager& saveManager);
  ~CombatSystem() = default;

  // Запрет копирования
  CombatSystem(const CombatSystem&) = delete;
  CombatSystem& operator=(const CombatSystem&) = delete;

  void startCombat(const std::string& enemyId);

  // Генерация вариантов ответов для UI
  std::vector<std::string> generateDialogueOptions();

 private:
  // Главный маршрутизатор хода игрока
  void handleActionSelected(const Evt_CombatActionSelected& event);

  // Действия игрока (возвращают true, если ход потрачен и нужно передать ход
  // врагу)
  bool executeMagicAttack(const std::string& skillId);
  bool executeManipulation(bool isAbsorb);
  bool executeSocialArgument(const std::string& loreId);

  // Ход врага и завершение
  void executeEnemyTurn();
  void endCombat(bool isVictory, bool isPeaceful);

  // Ссылки на системы
  EventBus& m_bus;
  Player& m_player;
  DataLoader& m_dataLoader;
  SaveManager& m_saveManager;

  // Состояние боя
  std::unique_ptr<Enemy> m_enemy;
  bool m_isCombatActive;

  // Кэшированное здоровье игрока (чтобы проверять смерть без нарушения
  // инкапсуляции)
  int m_currentPlayerHp;
};

#endif  // SOLIS_COMBAT_SYSTEM_H
