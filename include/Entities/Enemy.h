#ifndef ENEMY_H
#define ENEMY_H

#include <string>
#include <vector>
#include "Mechanics/StatusEffect.h"
#include "Utils/GameConstans.h"

namespace solis {

enum class Trait {
    Fanatical = 0,
    Executive = 1,
    Mercenary = 2,
    Cynical = 3,
    Empathetic = 4,
    Cowardly = 5,
    Exhausted = 6,
    Embittered = 7
};

struct EnemyTraitInfo {
    Trait trait;
    std::string trait_name_hidden;
    std::string intro_text;
};

class Enemy {
public:
    Enemy();
    virtual ~Enemy() = default;

    virtual bool isBoss() const { return false; }

    void takeDamage(int damage);
    
    // Getters
    const std::string& getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    int getMaxHp() const { return m_maxHp; }
    int getCurrentHp() const { return m_currentHp; }
    int getBaseDamage() const { return m_baseDamage; }
    float getDamageModifier() const { return m_damageModifier; }
    float getDefenseModifier() const { return m_defenseModifier; }
    Trait getTrait() const { return m_trait; }
    const std::string& getTraitNameHidden() const { return m_traitNameHidden; }
    const std::string& getIntroText() const { return m_introText; }
    int getEmpathyRevealThreshold() const { return m_empathyRevealThreshold; }

    // Setters
    void setId(const std::string& id) { m_id = id; }
    void setName(const std::string& name) { m_name = name; }
    void setMaxHp(int hp) { m_maxHp = hp; m_currentHp = hp; }
    void setBaseDamage(int damage) { m_baseDamage = damage; }
    void setSparksReward(int reward) { m_sparksReward = reward; }
    void setTrait(Trait t, const std::string& hiddenName, const std::string& intro) {
        m_trait = t;
        m_traitNameHidden = hiddenName;
        m_introText = intro;
    }

    void setDefenseModifier(float mod) { m_defenseModifier = mod; }
    void setDamageModifier(float mod) { m_damageModifier = mod; }
    void setEmpathyRevealThreshold(int val) { m_empathyRevealThreshold = val; }

    bool isDrainable() const { return m_isDrainable; }
    void setDrainable(bool drainable) { m_isDrainable = drainable; }

    void addStatusEffect(const StatusEffect& effect) { m_activeEffects.push_back(effect); }
    const std::vector<StatusEffect>& getStatusEffects() const { return m_activeEffects; }
    void processTurnEffects();

private:
    std::string m_id;
    std::string m_name;
    int m_maxHp = 50;
    int m_currentHp = 50;
    int m_baseDamage = 10;
    int m_sparksReward = 10;
    
    Trait m_trait = Trait::Executive;
    std::string m_traitNameHidden;
    std::string m_introText;

    float m_damageModifier = 1.0f;
    float m_defenseModifier = 1.0f;

    int m_empathyRevealThreshold = constants::EMPATHY_DEFAULT_REVEAL_THRESHOLD; 
    bool m_isDrainable = true; 

    std::vector<StatusEffect> m_activeEffects;
};

} // namespace solis

#endif // ENEMY_H
