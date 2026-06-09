#ifndef BOSS_ENEMY_H
#define BOSS_ENEMY_H

#include "Entities/Enemy.h"

namespace solis {

struct BossPhase {
    int willpower_left;
    Trait target_trait;
    std::string target_trait_name;
    std::string boss_replica;
    std::string success_reply;
    std::string fail_reply;
    int aggression_turns;
};

class BossEnemy : public Enemy {
public:
    BossEnemy();

    void addPhase(const BossPhase& phase) { m_phases.push_back(phase); }
    void updatePhase();
    const BossPhase& getCurrentPhase() const { return m_phases[m_currentPhaseIndex]; }

    int getCurrentWillpower() const { return m_currentWillpower; }
    void reduceWillpower() { if (m_currentWillpower > 0) m_currentWillpower--; updatePhase(); }
    void setMaxWillpower(int wp) { m_maxWillpower = wp; m_currentWillpower = wp; }
    
    int getAggressionTurns() const { return m_aggressionTurns; }
    void setAggressionTurns(int turns) { m_aggressionTurns = turns; }
    void decrementAggression() { if (m_aggressionTurns > 0) m_aggressionTurns--; }

    bool isBoss() const override { return true; }

private:
    std::vector<BossPhase> m_phases;
    int m_currentPhaseIndex = -1;
    
    int m_maxWillpower = 3;
    int m_currentWillpower = 3;
    int m_aggressionTurns = 0;
};

} // namespace solis

#endif // BOSS_ENEMY_H
