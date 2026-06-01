#include "Gameplay/CombatSystem.h"

#include <algorithm>
#include <random>

#include "Core/EventBus.h"
#include "Data/DataLoader.h"
#include "Data/SaveManager.h"
#include "Gameplay/Enemy.h"
#include "Gameplay/Player.h"
#include "Utils/GameConstans.h"

CombatSystem::CombatSystem(EventBus& bus, Player& player,
                           DataLoader& dataLoader, SaveManager& saveManager)
    : m_bus(bus),
      m_player(player),
      m_dataLoader(dataLoader),
      m_saveManager(saveManager),
      m_isCombatActive(false),
      m_currentPlayerHp(100)  // Инициализация с запасом
{
  // Подписка на действия игрока из UI
  m_bus.subscribe<Evt_CombatActionSelected>(
      [this](const Evt_CombatActionSelected& event) {
        handleActionSelected(event);
      });

  // Отслеживаем ХП игрока, так как прямого геттера в Player.h нет
  // (Строго следуем паттерну слабого связывания - данные берем из шины)
  m_bus.subscribe<Evt_PlayerHPChanged>(
      [this](const Evt_PlayerHPChanged& event) {
        m_currentPlayerHp = event.currentHP;
      });
}

void CombatSystem::startCombat(const std::string& enemyId) {
  // 1. Создаем врага через DataLoader
  m_enemy.reset(new Enemy(m_bus, m_dataLoader.getEnemy(enemyId)));
  m_isCombatActive = true;

  // 2. Публикуем логи
  m_bus.publish(
      Evt_LogMessage{"Бой начался: " + m_enemy->getName(), ColorID::Red});

  // 3. Форсируем апдейт UI
  m_enemy->forceUpdateUI();
  m_player.forceUpdateUI();
}

void CombatSystem::handleActionSelected(const Evt_CombatActionSelected& event) {
  if (!m_isCombatActive || !m_enemy) return;

  bool turnConsumed = false;

  // Маршрутизация действия
  if (event.actionCategory == "Attack") {
    turnConsumed = executeMagicAttack(event.actionDetail);
  } else if (event.actionCategory == "Manipulate_Absorb") {
    turnConsumed = executeManipulation(true);
  } else if (event.actionCategory == "Manipulate_Give") {
    turnConsumed = executeManipulation(false);
  } else if (event.actionCategory == "Social") {
    turnConsumed = executeSocialArgument(event.actionDetail);
  }

  // Если ход игрока прошел успешно и враг еще жив (и не сдался) — ход врага
  if (turnConsumed && m_enemy && !m_enemy->isDead() &&
      !m_enemy->isSociallyDefeated()) {
    executeEnemyTurn();
  }
}

bool CombatSystem::executeMagicAttack(const std::string& skillId) {
  // TODO: В будущем расчет урона и жара можно привязать к конкретному скиллу по
  // skillId
  int heatCost = GameConstants::MAGIC_ATTACK_BASE_HEAT_COST;
  int damage = GameConstants::MAGIC_ATTACK_BASE_DAMAGE;

  m_player.addHeat(heatCost);
  m_enemy->takeDamage(damage);

  m_bus.publish(Evt_LogMessage{"Вы атакуете магией на " +
                                   std::to_string(damage) +
                                   " урона! Ваш Жар вырос.",
                               ColorID::Red});

  if (m_enemy->isDead()) {
    endCombat(true, false);
  }

  return true;
}

bool CombatSystem::executeManipulation(bool isAbsorb) {
  if (isAbsorb) {
    m_player.addHeat(GameConstants::MANIPULATE_ABSORB_HEAT_COST);
    m_enemy->applyArmorModifier(GameConstants::MANIPULATE_ABSORB_ARMOR_DEBUFF);
    m_bus.publish(Evt_LogMessage{
        "Вы поглощаете энергию! Броня врага ослаблена, но Жар вырос.",
        ColorID::Default});
  } else {
    m_player.reduceHeat(GameConstants::MANIPULATE_GIVE_HEAT_REDUCTION);
    m_player.heal(GameConstants::MANIPULATE_GIVE_HEAL);
    m_enemy->applyDamageModifier(GameConstants::MANIPULATE_GIVE_DAMAGE_BUFF);
    m_bus.publish(Evt_LogMessage{
        "Вы отдаете тепло! Вы исцелены, Жар спал, но враг стал бить сильнее.",
        ColorID::Green});
  }

  return true;
}

bool CombatSystem::executeSocialArgument(const std::string& loreId) {
  if (m_enemy->isDialogueBlocked()) {
    m_bus.publish(Evt_LogMessage{
        "Враг не слушает вас! Попробуйте другой подход.", ColorID::Default});
    return false;  // Отменяем ход, не передаем его врагу
  }

  const auto& lore = m_dataLoader.getLore(loreId);

  // Сравнение ассоциированной черты из Лора с тайной чертой врага
  if (lore.associatedTrait == m_enemy->getHiddenTrait()) {
    m_enemy->reduceWillpower(1);
    m_bus.publish(Evt_LogMessage{"Убедительный аргумент! Воля врага дрогнула.",
                                 ColorID::Blue});

    if (m_enemy->isSociallyDefeated()) {
      m_bus.publish(Evt_LogMessage{
          "Враг опускает оружие. Вы убедили его сдаться.", ColorID::Green});
      endCombat(true, true);
    }
  } else {
    m_enemy->applyDamageModifier(GameConstants::SOCIAL_FAIL_DAMAGE_BUFF);
    m_enemy->blockDialogue(GameConstants::SOCIAL_FAIL_DIALOGUE_BLOCK_TURNS);
    m_bus.publish(Evt_LogMessage{
        "Неверный аргумент! Враг в ярости и отказывается говорить.",
        ColorID::Red});
  }

  return true;
}

void CombatSystem::executeEnemyTurn() {
  // Расчет и нанесение урона Игроку
  int dmg = m_enemy->calculateAttackDamage();
  m_player.takeDamage(dmg);

  m_bus.publish(Evt_LogMessage{
      "Враг наносит вам " + std::to_string(dmg) + " урона.", ColorID::Red});

  // Внутренние процессы тика хода
  m_enemy->tickTurn();
  m_player.processTurnEnd();  // Нанесение урона от перегрева если Жар > 85%

  // Проверяем состояние игрока (закешированное через Evt_PlayerHPChanged)
  if (m_currentPlayerHp <= 0) {
    m_bus.publish(Evt_LogMessage{"Вы были повержены в бою...", ColorID::Red});
    endCombat(false, false);
  }
}

void CombatSystem::endCombat(bool isVictory, bool isPeaceful) {
  if (isVictory) {
    // Проверка на Meltdown (Мгновенная смерть от >100% Жара при победе)
    m_player.checkPostCombatMeltdown();

    if (m_currentPlayerHp <= 0) {
      m_bus.publish(Evt_LogMessage{
          "Победа... но критический перегрев уничтожил вас.", ColorID::Red});
      isVictory = false;
    } else {
      // Если игрок выжил - обновляем счетчики
      auto& profile = m_saveManager.getProfile();

      if (isPeaceful) {
        profile.mercyCounter += 1;
        profile.bloodCounter = std::max(0, profile.bloodCounter - 1);
      } else {
        profile.bloodCounter += 1;
      }

      // Сообщаем системе о новых счетчиках (например, для UI)
      m_bus.publish(
          Evt_UpdateCounters{profile.bloodCounter, profile.mercyCounter});
    }
  }

  m_isCombatActive = false;
  m_enemy.reset();  // Очищаем противника

  m_bus.publish(Evt_LogMessage{"Бой завершен.", ColorID::Yellow});
  m_bus.publish(Evt_CombatEnd{isVictory});
}

std::vector<std::string> CombatSystem::generateDialogueOptions() {
  if (!m_enemy) return {};

  const auto& unlockedLore = m_saveManager.getProfile().unlockedLoreIds;

  std::vector<std::string> finalOptions;
  std::vector<std::string> incorrectPool;
  std::string correctLoreId;

  // Сортировка открытого лора на "верный" и "ошибочный"
  for (const auto& loreId : unlockedLore) {
    if (m_dataLoader.getLore(loreId).associatedTrait ==
        m_enemy->getHiddenTrait()) {
      correctLoreId = loreId;
    } else {
      incorrectPool.push_back(loreId);
    }
  }

  // Всегда стараемся добавить 1 правильный ответ, если он у нас открыт
  if (!correctLoreId.empty()) {
    finalOptions.push_back(correctLoreId);
  }

  // Подготовка случайной генерации
  std::random_device rd;
  std::mt19937 gen(rd());

  // Перемешиваем неправильные ответы
  std::shuffle(incorrectPool.begin(), incorrectPool.end(), gen);

  // Добираем максимум 3 неправильных (чтобы всего было 4)
  for (size_t i = 0; i < incorrectPool.size() && finalOptions.size() < 4; ++i) {
    finalOptions.push_back(incorrectPool[i]);
  }

  // Перемешиваем финальный пул ответов, чтобы верный ответ не был всегда первым
  std::shuffle(finalOptions.begin(), finalOptions.end(), gen);

  return finalOptions;
}
