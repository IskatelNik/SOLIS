#ifndef COMBAT_STATE_H
#define COMBAT_STATE_H

#include "Core/State.h"
#include "Core/StateMachine.h"
#include "UI/UIBox.h"
#include "Entities/Enemy.h"
#include "Entities/Player.h"
#include <memory>
#include <string>
#include <list>

namespace solis {

enum class CombatMenu {
    Main,
    Attack,
    ConfirmSkill,
    Manipulation,
    Negotiate
};

class CombatState : public State {
public:
    CombatState(sf::RenderWindow& window, StateMachine& stateMachine, std::unique_ptr<Enemy> enemy);

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    void updateUI();
    void logMessage(const std::string& msg);
    void enemyTurn();
    void checkEndCombat();
    void endCombat(bool victory);

    sf::RenderWindow& m_window;
    StateMachine& m_stateMachine;
    std::unique_ptr<Enemy> m_enemy;
    Player& m_player;

    std::unique_ptr<UIBox> m_topBar;
    std::unique_ptr<UIBox> m_logDisplay;
    std::unique_ptr<UIBox> m_enemyDisplay;
    std::unique_ptr<UIBox> m_actionMenu;
    std::unique_ptr<UIBox> m_playerStatus;

    CombatMenu m_currentMenu = CombatMenu::Main;
    int m_selectedSkillIndex = -1;
    std::list<std::string> m_logs;
    bool m_playerTurn = true;
    bool m_isCombatOver = false;
    bool m_keyHeld = false;
};

} // namespace solis

#endif // COMBAT_STATE_H
