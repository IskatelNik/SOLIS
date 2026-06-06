#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "Mechanics/Skill.h"
#include "Mechanics/StatusEffect.h"
#include "Utils/GameConstans.h"

namespace solis {

class Player {
public:
    Player();

    void takeDamage(int damage);
    void heal(int amount);
    void addHeat(float amount);
    void reduceHeat(float amount);

    // Эмпатия (MVP 3 Update)
    int getEmpathy() const { return m_empathyLevel; }
    void addEmpathy(int amount) { m_empathyLevel += amount; }
    void setEmpathy(int val) { m_empathyLevel = val; }

    // Getters
    int getMaxHp() const { return m_maxHp; }
    int getCurrentHp() const { return m_currentHp; }
    float getHeat() const { return m_heat; }
    const std::vector<Skill>& getActiveSkills() const { return m_activeSkills; }
    
    // For MVP 2 setup
    void setMaxHp(int val) { m_maxHp = val; m_currentHp = val; }
    void addSkill(const Skill& skill) { m_activeSkills.push_back(skill); }

private:
    int m_maxHp = constants::PLAYER_DEFAULT_MAX_HP;
    int m_currentHp = constants::PLAYER_DEFAULT_MAX_HP;
    float m_heat = 0.0f;
    int m_empathyLevel = 0;

    std::vector<Skill> m_activeSkills;
};

} // namespace solis

#endif // PLAYER_H
