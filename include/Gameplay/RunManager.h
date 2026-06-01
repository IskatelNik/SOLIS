#ifndef RUN_MANAGER_H
#define RUN_MANAGER_H

#include <random>
#include <string>
#include <vector>

// Предварительные объявления классов, чтобы избежать циклических зависимостей
class EventBus;
class Player;
class DataLoader;
class SaveManager;
struct Evt_DoorSelected;
struct Evt_CombatEnd;

enum class LevelID { Isolator = 1, Manufactory = 2, Labs = 3, Furnace = 4 };
enum class RoomType { Combat, Technical, Archive, Quest };

struct DoorOption {
  RoomType type;
  std::string hintText;
  std::string contentId;  // ID врага или лора
};

class RunManager {
 public:
  // Внедрение зависимостей (Dependency Injection) через ссылки
  RunManager(EventBus& bus, Player& player, DataLoader& dataLoader,
             SaveManager& saveManager);
  ~RunManager() = default;

  // Запрет копирования и перемещения
  RunManager(const RunManager&) = delete;
  RunManager& operator=(const RunManager&) = delete;
  RunManager(RunManager&&) = delete;
  RunManager& operator=(RunManager&&) = delete;

  // Главные методы управления забегом
  void generateAndStartRun(unsigned int seed);
  void generateNextDoors();

  // Обработчики событий (коллбеки)
  void handleDoorSelected(const Evt_DoorSelected& event);
  void handleCombatEnd(const Evt_CombatEnd& event);

  // Геттеры для модификаторов в других системах
  LevelID getCurrentLevel() const;

 private:
  void enterRoom(const DoorOption& room);
  void advanceLevelIfNeeded();

  std::string getRandomHint(RoomType type);
  std::string getRandomEnemyId();
  std::string getRandomLoreId();

 private:
  EventBus& m_bus;
  Player& m_player;
  DataLoader& m_dataLoader;
  SaveManager& m_saveManager;

  std::mt19937 m_rng;
  LevelID m_currentLevel;
  int m_roomsClearedInLevel;
  std::vector<DoorOption> m_currentDoors;
};

#endif  // RUN_MANAGER_H
