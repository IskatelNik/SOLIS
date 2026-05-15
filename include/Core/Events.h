#ifndef EVENTS
#define EVENTS

#include <string>
#include <vector>

// ==========================================
// Перечисления (Enums)
// ==========================================

enum class GameStateID { MainMenu, Hub, Explore, Combat, GameOver };

enum class ColorID {
  Default,  // Заменяет White для большей абстракции
  Red,
  Green,
  Yellow,
  Blue
};

// ==========================================
// 1. События состояния игры
// ==========================================

struct Evt_ChangeState {
  GameStateID newState;
};

// ==========================================
// 2. Геймплейные события (Игрок и Бой)
// ==========================================

struct Evt_PlayerHPChanged {
  int currentHP;
  int maxHP;
};

struct Evt_PlayerHeatChanged {
  int currentHeat;
  int maxHeat;
  bool isOverheated;
};

struct Evt_EnemyHPChanged {
  int currentHP;
  int maxHP;
};

struct Evt_EnemyWillpowerChanged {
  int currentWill;
  int maxWill;
};

struct Evt_CombatStart {
  std::string enemyId;
};

struct Evt_CombatEnd {
  bool isVictory;
};

struct Evt_UpdateCounters {
  int bloodCounter;
  int mercyCounter;
};

// ==========================================
// 3. UI и Навигационные События
// ==========================================

struct Evt_LogMessage {
  std::string text;
  ColorID color;
};

struct Evt_ClearLog {
  // Пустое событие (сигнал)
};

struct Evt_ShowRoomDoors {
  std::vector<std::string> doorDescriptions;
};

struct Evt_DoorSelected {
  int doorIndex;
};

struct Evt_CombatActionSelected {
  std::string actionCategory;  // "Attack", "Manipulate", "Social"
  std::string actionDetail;    // ID скилла или ID лора
};

#endif  // !EVENTS
