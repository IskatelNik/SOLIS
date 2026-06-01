#ifndef PLAYER
#define PLAYER

#include <string>
#include <vector>

// Предварительное объявление шины событий (EventBus),
// чтобы не тянуть лишние зависимости в заголовок.
class EventBus;

class Player {
 public:
  explicit Player(EventBus& bus);

  // Инициализация
  void resetForNewRun(int baseMaxHp, int baseMaxHeat);

  // Управление здоровьем (ХП)
  void takeDamage(int amount);
  void heal(int amount);

  // Управление Жаром
  void addHeat(int amount);
  void reduceHeat(int amount);
  bool isOverloaded() const;
  void processTurnEnd();
  void checkPostCombatMeltdown();

  // Экипировка и Артефакты
  void equipSkills(const std::vector<std::string>& skillIds);
  const std::vector<std::string>& getEquippedSkills() const;
  void addArtifact(const std::string& artifactId);

  // Получение инвентаря для боевой системы
  const std::vector<std::string>& getArtifacts() const;

  // Принудительное обновление UI
  void forceUpdateUI();

 private:
  EventBus& m_bus;

  int m_currentHp;
  int m_maxHp;
  int m_currentHeat;
  int m_maxHeat;

  std::vector<std::string> m_equippedSkills;
  std::vector<std::string> m_artifacts;
};
#endif  // !PLAYER
