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
    options.push_back({correctLore.dialogue_option_text, target, isActuallyCorrect, !isActuallyCorrect});

    // 2. Нейтральные ответы (i-1, i+1)
    int n1 = (target - 1 + 8) % 8;
    int n2 = (target + 1) % 8;
    
    LoreItem neutral1 = getBestLoreForTrait(n1, currentLevel, unlocked);
    options.push_back({neutral1.dialogue_option_text, n1, false, true});
    
    LoreItem neutral2 = getBestLoreForTrait(n2, currentLevel, unlocked);
    options.push_back({neutral2.dialogue_option_text, n2, false, true});

    // 3. Неверный ответ (случайный из оставшихся)
    std::vector<int> others;
    for(int i=0; i<8; ++i) {
        if(i != target && i != n1 && i != n2) others.push_back(i);
    }
    
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(others.begin(), others.end(), gen);
    
    int wrongTrait = others[0];
    LoreItem wrongLore = getBestLoreForTrait(wrongTrait, currentLevel, unlocked);
    // Неверный ответ никогда не бывает верным или нейтральным
    options.push_back({wrongLore.dialogue_option_text, wrongTrait, false, false});

    // 4. Перемешиваем
    std::shuffle(options.begin(), options.end(), gen);

    return options;
}

LoreItem DialogueGenerator::getBestLoreForTrait(int traitIndex, int currentLevel, const std::vector<std::string>& unlockedIds) {
    const auto& allLore = DataManager::getInstance().getLore();
    
    // Ищем только в разблокированном лоре для этой черты
    for (const auto& id : unlockedIds) {
        const auto& lore = allLore.at(id);
        if (lore.target_trait == traitIndex) {
            return lore;
        }
    }
    
    // Если лор не найден или не разблокирован — возвращаем общую заглушку
    return getPlaceholder();
}

LoreItem DialogueGenerator::getPlaceholder() {
    const auto& allLore = DataManager::getInstance().getLore();
    for (auto const& [id, lore] : allLore) {
        if (lore.target_trait == -1) return lore;
    }
    return {"placeholder", 1, -1, "...", "???"};
}

} // namespace solis
