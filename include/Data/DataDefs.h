#ifndef DATADEFS_HPP
#define DATADEFS_HPP

#include <nlohmann/json.hpp>
#include <string>

// 1. Шаблон врага
struct EnemyDef {
  std::string id;
  std::string name;
  int maxHp;
  int maxWill;
  std::string hiddenTrait;
  int baseDamage;
};
// Автоматическая генерация to_json / from_json
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EnemyDef, id, name, maxHp, maxWill,
                                   hiddenTrait, baseDamage)

// 2. Кусочек лора (Угли Мира)
struct LoreDef {
  std::string id;
  std::string title;
  std::string text;
  std::string associatedTrait;
  std::string argumentText;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LoreDef, id, title, text, associatedTrait,
                                   argumentText)

// 3. Шаблон комнаты (на будущее)
struct RoomDef {
  std::string id;
  std::string description;
  std::string roomType;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(RoomDef, id, description, roomType)

#endif  // DATADEFS_HPP
