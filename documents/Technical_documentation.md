# ТЕХНИЧЕСКАЯ ДОКУМЕНТАЦИЯ (TDD) — Проект SOLIS

Данный документ описывает исчерпывающую техническую архитектуру, UI-дизайн и игровую логику для консольной игры **SOLIS** (C++17, SFML 3.0.2, nlohmann/json).

## 11. Система Прогрессии

### 11.2. Эмпатия и Раскрытие Чертов
*   **Эмпатия (Empathy):** Постоянная характеристика игрока. Не сбрасывается при смерти.
    *   `+10` за верный ответ в социальном бою.
    *   `+5` за нейтральный ответ.
    *   `+2` за неверный ответ.
*   **Раскрытие черты:** Черта врага (`trait_name_hidden`) заменяет `???` только если `Player.empathy >= Enemy.empathy_reveal_threshold`.

## 15. Формулы и Баланс

### 15.4. Формула Урона (Skills)
Урон навыков масштабируется от текущего уровня Жара игрока. Расчет происходит ДО того, как навык прибавит свой собственный Жар.
*   **Множитель Жара:** `float heatMultiplier = 1.0f + (Player.heat / 100.0f);` (от 1.0 до 2.0+).
*   **Итоговый урон:** `float finalDamage = skill.base_damage * heatMultiplier * Enemy.defenseModifier;`

## 16. Структура Данных (Примеры JSON)

### 16.1. `save.json` (Глобальная мета-прогрессия)
```json
{
  "solis_sparks": 120,
  "empathy_level": 45,
  "unlocked_lore": ["lore_fanatical", "lore_mercenary"],
  "unlocked_upgrades": ["hp_boost_1", "skill_fireball"],
  "equipped_skills": ["skill_fireball", "skill_shield"],
  "blood_counter": 12,
  "mercy_counter": 5
}
```

### 16.2. `enemies.json` (База врагов)
```json
{
  "enemies": [
    {
      "id": "enemy_guard",
      "name": "Стражник",
      "max_hp": 50,
      "base_damage": 10,
      "empathy_reveal_threshold": 25,
      "possible_traits": [
        {
          "trait": 1,
          "trait_name_hidden": "Исполнительный",
          "intro_text": "..."
        }
      ]
    }
  ]
}
```
