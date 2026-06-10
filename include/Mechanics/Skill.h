#ifndef SKILL_H
#define SKILL_H

#include <string>
#include <vector>
#include <SFML/System/String.hpp>

namespace solis {

struct SkillEffect {
    std::string target; // "self" or "enemy"
    std::string type;   // "heal_flat", "debuff_damage_multiplier", etc.
    float value;
    int duration_turns;
};

struct Skill {
    std::string id;
    std::string name;
    std::string description;
    float heat_cost_added;
    int base_damage;
    std::vector<SkillEffect> effects;
};

} // namespace solis

#endif // SKILL_H
