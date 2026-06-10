#ifndef DIALOGUE_GENERATOR_H
#define DIALOGUE_GENERATOR_H

#include "Managers/DataManager.h"
#include "Entities/Enemy.h"
#include <vector>
#include <string>

namespace solis {

class DialogueGenerator {
public:
    struct DialogueOption {
        std::string text;
        int traitIndex; // The trait this text belongs to
        bool isCorrect;
        bool isNeutral;
    };

    static std::vector<DialogueOption> generateOptions(Trait enemyTrait, int currentLevel);

private:
    static LoreItem getBestLoreForTrait(int traitIndex, int currentLevel, const std::vector<std::string>& unlockedIds);
    static LoreItem getPlaceholder(int currentLevel);
};

} // namespace solis

#endif // DIALOGUE_GENERATOR_H
