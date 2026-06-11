#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "Mechanics/Skill.h"
#include "Mechanics/StatusEffect.h"
#include "Mechanics/Artifact.h"
#include "Utils/GameConstans.h"

namespace solis {

class Player {
public:
    Player();

    void takeDamage(int damage);
    void heal(int amount);
    void addHeat(float amount);
    void reduceHeat(float amount);

    // MVP 5: Modifiers
    float calculateHeatGain(float baseAmount) const;
    void addArtifact(const Artifact& artifact) { 
        m_artifacts.push_back(artifact); 
        if (artifact.modifier_type == "max_hp_flat") {
            m_maxHp += static_cast<int>(artifact.value);
            m_currentHp += static_cast<int>(artifact.value);
        }
    }
    void addStatusEffect(const StatusEffect& effect) { m_activeEffects.push_back(effect); }
    const std::vector<StatusEffect>& getStatusEffects() const { return m_activeEffects; }
    std::string removeRandomArtifact(); // MVP 5 Biome 3 Update
    void processTurnEffects();

    // Эмпатия (MVP 3 Update)
    int getEmpathy() const { return m_empathyLevel; }
    void addEmpathy(int amount) { m_empathyLevel += amount; }
    void setEmpathy(int val) { m_empathyLevel = val; }

    // Getters
    int getMaxHp() const { return m_maxHp; }
    int getCurrentHp() const { return m_currentHp; }
    float getHeat() const { return m_heat; }
    const std::vector<Skill>& getActiveSkills() const { return m_activeSkills; }
    
    float getOverloadThreshold() const;
    int getBaseDamageBonus() const;
    float getHeatGainMultiplier() const;
    float getDefenseMultiplier() const;
    
    // For MVP 2 setup
    void setMaxHp(int val) { m_maxHp = val; m_currentHp = val; }
    void setBaseOverloadThreshold(float val) { m_baseOverloadThreshold = val; }
    void setBaseDamageBonus(int val) { m_baseDamageBonus = val; }
    void setBaseHeatGainMultiplier(float val) { m_baseHeatGainMultiplier = val; }
    void addSkill(const Skill& skill) { m_activeSkills.push_back(skill); }

private:
    int m_maxHp = constants::PLAYER_DEFAULT_MAX_HP;
    int m_currentHp = constants::PLAYER_DEFAULT_MAX_HP;
    float m_heat = 0.0f;
    int m_empathyLevel = 0;

    // Base stats (modified by permanent upgrades at start of run)
    float m_baseOverloadThreshold = constants::HEAT_OVERLOAD_THRESHOLD;
    int m_baseDamageBonus = 0;
    float m_baseHeatGainMultiplier = 1.0f;

    std::vector<Skill> m_activeSkills;
    std::vector<Artifact> m_artifacts;
    std::vector<StatusEffect> m_activeEffects;
};

} // namespace solis

#endif // PLAYER_H
