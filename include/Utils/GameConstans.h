#ifndef GAME_CONSTANTS_H
#define GAME_CONSTANTS_H

#include <SFML/Graphics/Color.hpp>

namespace solis {
namespace constants {

    // --- UI LAYOUT (Percentages 0.0 to 1.0) ---
    constexpr float UI_TOPBAR_HEIGHT = 0.10f;
    constexpr float UI_MAIN_DISPLAY_HEIGHT = 0.55f;
    constexpr float UI_ACTION_MENU_HEIGHT = 0.35f;
    constexpr float UI_ACTION_SPLIT_LEFT = 0.70f;
    constexpr float UI_ACTION_SPLIT_RIGHT = 0.30f;
    constexpr float UI_PADDING = 10.0f;
    constexpr float UI_BAR_HEIGHT = 15.0f;

    // --- PLAYER BASE STATS ---
    constexpr int PLAYER_DEFAULT_MAX_HP = 100;
    constexpr float HEAT_MAX = 100.0f;
    constexpr float HEAT_OVERLOAD_THRESHOLD = 85.0f;
    constexpr float HEAT_DAMAGE_MULT = 0.5f; // Damage per Heat point above threshold
    constexpr float HEAT_DAMAGE_MIN_MULT = 1.0f;

    // --- EMPATHY SYSTEM ---
    constexpr int EMPATHY_REWARD_CORRECT = 10;
    constexpr int EMPATHY_REWARD_NEUTRAL = 5;
    constexpr int EMPATHY_REWARD_WRONG = 2;
    constexpr int EMPATHY_DEFAULT_REVEAL_THRESHOLD = 20;

    // --- EXPLORATION & ROOMS ---
    constexpr int MAX_ROOM_OPTIONS = 3;
    constexpr float TERMINAL_HEAT_VENT_PERCENT = 0.90f;
    constexpr float TERMINAL_HEAL_PERCENT = 0.15f;

    // --- COMBAT MANIPULATION ---
    constexpr float MANIP_TAKE_HEAT_GAIN = 15.0f;
    constexpr float MANIP_TAKE_DEF_DEBUFF = -0.2f;
    constexpr float MANIP_GIVE_HEAT_VENT_PERCENT = 0.20f;
    constexpr float MANIP_GIVE_HEAL_MULT = 0.5f; // Heal per vented heat
    constexpr float MANIP_GIVE_DMG_BUFF = 0.2f;

    // --- COLORS ---
    const sf::Color COLOR_UI_BG_DARK = sf::Color(20, 20, 20);
    const sf::Color COLOR_UI_BG_LIGHT = sf::Color(30, 30, 30);
    const sf::Color COLOR_UI_OUTLINE = sf::Color::White;
    const sf::Color COLOR_HEAT_NORMAL = sf::Color::Yellow;
    const sf::Color COLOR_HEAT_OVERLOAD = sf::Color::Red;
    const sf::Color COLOR_LORE_DISCOVERY = sf::Color::Cyan;

} // namespace constants
} // namespace solis

#endif // GAME_CONSTANTS_H
