#ifndef SAVE_PROFILE_H
#define SAVE_PROFILE_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

// Структура, хранящая весь глобальный прогресс игрока
struct SaveProfile {
    int sparksCount = 0;
    int bloodCounter = 0;
    int mercyCounter = 0;
    int ideologicalCompass = 0;

    std::vector<std::string> unlockedLoreIds;
    std::vector<std::string> unlockedSkillIds;
    std::vector<std::string> equippedSkillIds;

    int maxRunReached = 0;
};

// Макрос позволяет автоматически парсить JSON в структуру и обратно.
// Благодаря _WITH_DEFAULT, если в старом сейве нет нового поля (например, maxRunReached), 
// оно возьмет дефолтное значение из структуры, и игра не упадет.
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
    SaveProfile,
    sparksCount,
    bloodCounter,
    mercyCounter,
    ideologicalCompass,
    unlockedLoreIds,
    unlockedSkillIds,
    equippedSkillIds,
    maxRunReached
)

#endif // SAVE_PROFILE_H