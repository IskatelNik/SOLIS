#include "Mechanics/DialogueGenerator.h"
#include "Managers/RunManager.h"
#include <algorithm>
#include <random>

namespace solis {

/**
 * @brief Генерирует 3-4 варианта ответа для социальной фазы боя.
 * Логика учитывает текущий уровень (биома) и наличие открытого лора.
 */
std::vector<DialogueGenerator::DialogueOption> DialogueGenerator::generateOptions(Trait enemyTrait, int currentLevel) {
    int target = static_cast<int>(enemyTrait);
    const auto& unlocked = RunManager::getInstance().getUnlockedLore();
    
    std::vector<DialogueOption> options;

    // 1. Формирование ПРАВИЛЬНОГО ответа
    LoreItem correctLore = getBestLoreForTrait(target, currentLevel, unlocked);
    // Если игрок еще не нашел нужный лор, вернется заглушка, которая не считается верной
    bool isActuallyCorrect = (correctLore.target_trait == target);
    options.push_back({correctLore.dialogue_option_text, target, isActuallyCorrect, false});

    static std::random_device rd;
    static std::mt19937 gen(rd());

    // 2. Модификатор Мануфактур (Уровень 2): Исключаем нейтральные ответы (повышенная сложность)
    if (currentLevel == 2) {
        std::vector<int> others;
        for(int i=0; i<8; ++i) {
            if(i != target) others.push_back(i);
        }
        std::shuffle(others.begin(), others.end(), gen);
        
        for (int j = 0; j < 3; ++j) {
            int wrongTrait = others[j];
            LoreItem wrongLore = getBestLoreForTrait(wrongTrait, currentLevel, unlocked);
            options.push_back({wrongLore.dialogue_option_text, wrongTrait, false, false});
        }
    } else {
        // 3. Стандартная генерация: 1 нейтральный вариант и 2 заведомо неверных
        int n1 = (target - 1 + 8) % 8;
        int n2 = (target + 1) % 8;
        
        std::uniform_int_distribution<> coin(0, 1);
        int chosenNeutral = (coin(gen) == 0) ? n1 : n2;
        
        LoreItem neutralLore = getBestLoreForTrait(chosenNeutral, currentLevel, unlocked);
        options.push_back({neutralLore.dialogue_option_text, chosenNeutral, false, true});

        std::vector<int> others;
        for(int i=0; i<8; ++i) {
            if(i != target && i != n1 && i != n2) others.push_back(i);
        }
        std::shuffle(others.begin(), others.end(), gen);
        
        for (int j = 0; j < 2; ++j) {
            int wrongTrait = others[j];
            LoreItem wrongLore = getBestLoreForTrait(wrongTrait, currentLevel, unlocked);
            options.push_back({wrongLore.dialogue_option_text, wrongTrait, false, false});
        }
    }

    // Перемешивание вариантов, чтобы правильный ответ не всегда был первым
    std::shuffle(options.begin(), options.end(), gen);

    return options;
}

/**
 * @brief Ищет наиболее подходящую реплику для конкретной черты на текущем уровне.
 * Если лор не открыт — возвращает заглушку текущего биома.
 */
LoreItem DialogueGenerator::getBestLoreForTrait(int traitIndex, int currentLevel, const std::vector<std::string>& unlockedIds) {
    const auto& allLore = DataManager::getInstance().getLore();
    
    for (const auto& id : unlockedIds) {
        if (allLore.find(id) != allLore.end()) {
            const auto& lore = allLore.at(id);
            if (lore.target_trait == traitIndex && lore.level == currentLevel) {
                return lore;
            }
        }
    }
    
    return getPlaceholder(currentLevel);
}

/**
 * @brief Возвращает "неизвестную" реплику (???) для текущего уровня.
 */
LoreItem DialogueGenerator::getPlaceholder(int currentLevel) {
    const auto& allLore = DataManager::getInstance().getLore();
    for (auto const& [id, lore] : allLore) {
        if (lore.target_trait == -1 && lore.level == currentLevel) return lore;
    }
    return {"placeholder", currentLevel, -1, "...", "???"};
}

} // namespace solis
