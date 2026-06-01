#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

namespace GameConstants {
// Бой: Магия
constexpr int MAGIC_ATTACK_BASE_HEAT_COST = 20;
constexpr int MAGIC_ATTACK_BASE_DAMAGE = 15;

// Бой: Манипуляция
constexpr int MANIPULATE_ABSORB_HEAT_COST = 25;
constexpr float MANIPULATE_ABSORB_ARMOR_DEBUFF = 1.5f;

constexpr int MANIPULATE_GIVE_HEAT_REDUCTION = 20;
constexpr int MANIPULATE_GIVE_HEAL = 15;
constexpr float MANIPULATE_GIVE_DAMAGE_BUFF = 1.5f;

// Бой: Социальный
constexpr int SOCIAL_FAIL_DIALOGUE_BLOCK_TURNS = 2;
constexpr float SOCIAL_FAIL_DAMAGE_BUFF = 1.5f;

constexpr int ROOMS_PER_LEVEL = 10;

// Количество снимаемого Жара в технической комнате
constexpr int TECHNICAL_ROOM_HEAT_REDUCTION = 50;

// Награда в Искрах (Sparks) за победу в обычной комнате
constexpr int COMBAT_VICTORY_SPARKS = 10;
}  // namespace GameConstants

#endif  // GAME_CONSTANTS_H
