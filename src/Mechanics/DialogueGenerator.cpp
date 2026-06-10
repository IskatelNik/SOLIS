#include "Mechanics/DialogueGenerator.h"
#include "Managers/RunManager.h"
#include <algorithm>
#include <random>

namespace solis {

std::vector<DialogueGenerator::DialogueOption> DialogueGenerator::generateOptions(Trait enemyTrait, int currentLevel) {
    int target = static_cast<int>(enemyTrait);
    const auto& unlocked = RunManager::getInstance().getUnlockedLore();
    
    std::vector<DialogueOption> options;

    // 1. Попытка получить верный ответ (i)
    LoreItem correctLore = getBestLoreForTrait(target, currentLevel, unlocked);
    // Если лор не открыт (вернулась заглушка с target_trait == -1), это НЕ считается верным ответом
    bool isActuallyCorrect = (correctLore.target_trait == target);
    options.push_back({correctLore.dialogue_option_text, target, isActuallyCorrect, false});

    static std::random_device rd;
    static std::mt19937 gen(rd());

    // MVP 5: Biome Modifiers (Level 2 disables neutral answers entirely)
    if (currentLevel == 2) {
        // На 2 уровне нет нейтральных ответов, генерируем 3 неверных
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
        // Стандартная генерация: 1 нейтральный, 2 неверных
        int n1 = (target - 1 + 8) % 8;
        int n2 = (target + 1) % 8;
        
        std::uniform_int_distribution<> coin(0, 1);
        int chosenNeutral = (coin(gen) == 0) ? n1 : n2;
        
        LoreItem neutralLore = getBestLoreForTrait(chosenNeutral, currentLevel, unlocked);
        options.push_back({neutralLore.dialogue_option_text, chosenNeutral, false, true});

        std::vector<int> others;
        for(int i=0; i<8; ++i) {
            // Исключаем целевую черту и обе нейтральные (чтобы случайно не выдать вторую нейтральную как неверную)
            if(i != target && i != n1 && i != n2) others.push_back(i);
        }
        std::shuffle(others.begin(), others.end(), gen);
        
        for (int j = 0; j < 2; ++j) {
            int wrongTrait = others[j];
            LoreItem wrongLore = getBestLoreForTrait(wrongTrait, currentLevel, unlocked);
            options.push_back({wrongLore.dialogue_option_text, wrongTrait, false, false});
        }
    }

    // 4. Перемешиваем
    std::shuffle(options.begin(), options.end(), gen);

    return options;
}

LoreItem DialogueGenerator::getBestLoreForTrait(int traitIndex, int currentLevel, const std::vector<std::string>& unlockedIds) {
    const auto& allLore = DataManager::getInstance().getLore();
    
    // Ищем в разблокированном лоре для этой черты (любого уровня)
    for (const auto& id : unlockedIds) {
        if (allLore.find(id) != allLore.end()) {
            const auto& lore = allLore.at(id);
            if (lore.target_trait == traitIndex) {
                return lore;
            }
        }
    }
    
    // Если лор не найден или не разблокирован — возвращаем общую заглушку для этого уровня
    return getPlaceholder(currentLevel);
}

LoreItem DialogueGenerator::getPlaceholder(int currentLevel) {
    const auto& allLore = DataManager::getInstance().getLore();
    for (auto const& [id, lore] : allLore) {
        if (lore.target_trait == -1 && lore.level == currentLevel) return lore;
    }
    // Фолбэк на случай отсутствия заглушки нужного уровня
    return {"placeholder", currentLevel, -1, "...", "???"};
}

} // namespace solis
