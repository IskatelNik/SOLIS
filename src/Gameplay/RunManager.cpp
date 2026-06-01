#include "Gameplay/RunManager.h"

#include <algorithm>

#include "Core/EventBus.h"
#include "Core/Events.h"
#include "Data/DataLoader.h"
#include "Data/SaveManager.h"
#include "Gameplay/Player.h"
#include "Utils/GameConstans.h"

// Статичные пулы подсказок для дверей
static const std::vector<std::string> HINTS_COMBAT = {
    "Слышны тяжелые шаги", "Впереди мерцает красный свет", "Пахнет гарью"};
static const std::vector<std::string> HINTS_TECHNICAL = {
    "Слышен гул механизмов", "Искрят оборванные провода",
    "Воздух здесь холоднее"};
static const std::vector<std::string> HINTS_ARCHIVE = {
    "Пахнет старой бумагой и пылью", "Видны ряды заброшенных стеллажей",
    "Слабое сияние гало-экрана"};
static const std::vector<std::string> HINTS_QUEST = {
    "Странные тени на стенах", "Необычная тишина", "Следы на полу"};

RunManager::RunManager(EventBus& bus, Player& player, DataLoader& dataLoader,
                       SaveManager& saveManager)
    : m_bus(bus),
      m_player(player),
      m_dataLoader(dataLoader),
      m_saveManager(saveManager),
      m_currentLevel(LevelID::Isolator),
      m_roomsClearedInLevel(0) {
  // Подписка на события UI и боя
  m_bus.subscribe<Evt_DoorSelected>(
      [this](const Evt_DoorSelected& e) { this->handleDoorSelected(e); });

  m_bus.subscribe<Evt_CombatEnd>(
      [this](const Evt_CombatEnd& e) { this->handleCombatEnd(e); });
}

void RunManager::generateAndStartRun(unsigned int seed) {
  m_rng.seed(seed);
  m_currentLevel = LevelID::Isolator;
  m_roomsClearedInLevel = 0;

  // Используем ColorID::Blue вместо отсутствующего White для системного
  // сообщения
  m_bus.publish(
      Evt_LogMessage{"Запуск протокола погружения...", ColorID::Blue});
  generateNextDoors();
}

void RunManager::generateNextDoors() {
  std::uniform_int_distribution<int> countDist(2, 3);
  int numDoors = countDist(m_rng);

  // Веса комнат (боевые должны попадаться чаще)
  std::uniform_int_distribution<int> typeDist(0, 5);

  bool validGeneration = false;
  while (!validGeneration) {
    m_currentDoors.clear();

    for (int i = 0; i < numDoors; ++i) {
      int typeRoll = typeDist(m_rng);
      RoomType rt;
      if (typeRoll <= 2)
        rt = RoomType::Combat;  // 50% шанс
      else if (typeRoll == 3)
        rt = RoomType::Technical;  // ~16% шанс
      else if (typeRoll == 4)
        rt = RoomType::Archive;  // ~16% шанс
      else
        rt = RoomType::Quest;  // ~16% шанс

      DoorOption option;
      option.type = rt;
      option.hintText = getRandomHint(rt);

      if (rt == RoomType::Combat) {
        option.contentId = getRandomEnemyId();
      } else if (rt == RoomType::Archive) {
        option.contentId = getRandomLoreId();
      } else {
        option.contentId = "none";
      }

      m_currentDoors.push_back(option);
    }

    // Ограничение: не допускаем 3-х одинаковых комнат на выбор
    if (numDoors == 3) {
      if (m_currentDoors[0].type == m_currentDoors[1].type &&
          m_currentDoors[1].type == m_currentDoors[2].type) {
        continue;  // Рероллим
      }
    }
    validGeneration = true;
  }

  // Подготовка данных для отправки в UI
  std::vector<std::string> doorDescriptions;
  doorDescriptions.reserve(m_currentDoors.size());
  for (const auto& door : m_currentDoors) {
    doorDescriptions.push_back(door.hintText);
  }

  m_bus.publish(Evt_LogMessage{"Перед вами несколько путей...", ColorID::Blue});
  m_bus.publish(Evt_ShowRoomDoors{doorDescriptions});
}

void RunManager::handleDoorSelected(const Evt_DoorSelected& event) {
  if (event.doorIndex < 0 ||
      event.doorIndex >= static_cast<int>(m_currentDoors.size())) {
    return;  // Защита от Out-of-bounds
  }
  enterRoom(m_currentDoors[event.doorIndex]);
}

void RunManager::enterRoom(const DoorOption& room) {
  m_roomsClearedInLevel++;
  advanceLevelIfNeeded();

  switch (room.type) {
    case RoomType::Combat:
      m_bus.publish(
          Evt_LogMessage{"Вы входите в опасную зону...", ColorID::Red});
      // Макро-цикл ждет Evt_CombatEnd
      m_bus.publish(Evt_CombatStart{room.contentId});
      break;

    case RoomType::Technical:
      m_bus.publish(Evt_LogMessage{"Вы находите терминал и сбрасываете Жар.",
                                   ColorID::Blue});
      m_player.reduceHeat(GameConstants::TECHNICAL_ROOM_HEAT_REDUCTION);
      generateNextDoors();
      break;

    case RoomType::Archive:
      m_bus.publish(Evt_LogMessage{
          "Взломан архив: найдена запись данных (" + room.contentId + ").",
          ColorID::Green});

      // Сохраняем Лор в актуальное поле SaveProfile
      m_saveManager.getProfile().unlockedLoreIds.push_back(room.contentId);
      generateNextDoors();
      break;

    case RoomType::Quest:
      // Заменили White на Blue
      m_bus.publish(Evt_LogMessage{"Пустая комната с остатками прежней жизни.",
                                   ColorID::Blue});
      generateNextDoors();
      break;
  }
}

void RunManager::handleCombatEnd(const Evt_CombatEnd& event) {
  if (event.isVictory) {
    m_bus.publish(
        Evt_LogMessage{"Бой окончен. Враг повержен.", ColorID::Green});

    // Исправлено: используем sparksCount вместо sparks
    m_saveManager.getProfile().sparksCount +=
        GameConstants::COMBAT_VICTORY_SPARKS;

    // Генерация лута
    std::string artifactId =
        "artifact_core_" +
        std::to_string(std::uniform_int_distribution<int>(1, 10)(m_rng));
    m_player.addArtifact(artifactId);
    m_bus.publish(
        Evt_LogMessage{"Получен артефакт: " + artifactId, ColorID::Yellow});

    // Возобновление макро-цикла забега
    generateNextDoors();
  } else {
    m_bus.publish(Evt_LogMessage{
        "Критическое повреждение. Системы отключаются...", ColorID::Red});
    m_bus.publish(Evt_ChangeState{GameStateID::GameOver});
  }
}

LevelID RunManager::getCurrentLevel() const { return m_currentLevel; }

void RunManager::advanceLevelIfNeeded() {
  if (m_roomsClearedInLevel > GameConstants::ROOMS_PER_LEVEL) {
    m_roomsClearedInLevel = 1;
    int currentLevelInt = static_cast<int>(m_currentLevel);

    if (currentLevelInt < static_cast<int>(LevelID::Furnace)) {
      m_currentLevel = static_cast<LevelID>(currentLevelInt + 1);
      m_bus.publish(Evt_LogMessage{"Вы спускаетесь глубже в комплекс...",
                                   ColorID::Yellow});
    }
  }
}

std::string RunManager::getRandomHint(RoomType type) {
  switch (type) {
    case RoomType::Combat: {
      std::uniform_int_distribution<size_t> dist(0, HINTS_COMBAT.size() - 1);
      return HINTS_COMBAT[dist(m_rng)];
    }
    case RoomType::Technical: {
      std::uniform_int_distribution<size_t> dist(0, HINTS_TECHNICAL.size() - 1);
      return HINTS_TECHNICAL[dist(m_rng)];
    }
    case RoomType::Archive: {
      std::uniform_int_distribution<size_t> dist(0, HINTS_ARCHIVE.size() - 1);
      return HINTS_ARCHIVE[dist(m_rng)];
    }
    case RoomType::Quest: {
      std::uniform_int_distribution<size_t> dist(0, HINTS_QUEST.size() - 1);
      return HINTS_QUEST[dist(m_rng)];
    }
  }
  return "Неизвестность...";
}

std::string RunManager::getRandomEnemyId() {
  static const std::vector<std::string> ENEMY_POOL = {
      "enemy_drone", "enemy_guard", "enemy_mutant", "enemy_turret"};
  std::uniform_int_distribution<size_t> dist(0, ENEMY_POOL.size() - 1);
  return ENEMY_POOL[dist(m_rng)];
}

std::string RunManager::getRandomLoreId() {
  const auto& allLore = m_dataLoader.getAllLore();
  if (allLore.empty()) {
    return "lore_unknown";
  }

  // Сортируем ключи, чтобы рандом был честным и 100% повторялся при одинаковом
  // Seed
  std::vector<std::string> keys;
  keys.reserve(allLore.size());
  for (const auto& pair : allLore) {
    keys.push_back(pair.first);
  }
  std::sort(keys.begin(), keys.end());

  std::uniform_int_distribution<size_t> dist(0, keys.size() - 1);
  return keys[dist(m_rng)];
}
