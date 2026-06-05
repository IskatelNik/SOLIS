#ifndef EXPLORATION_STATE_H
#define EXPLORATION_STATE_H

#include "Core/State.h"
#include "Core/StateMachine.h"
#include "UI/UIBox.h"
#include "Managers/DataManager.h"
#include <memory>
#include <vector>

namespace solis {

class ExplorationState : public State {
public:
    ExplorationState(sf::RenderWindow& window, StateMachine& stateMachine);

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    void generateNextStep();

    sf::RenderWindow& m_window;
    StateMachine& m_stateMachine;

    std::unique_ptr<UIBox> m_topBar;
    std::unique_ptr<UIBox> m_mainDisplay;
    std::unique_ptr<UIBox> m_actionMenu;

    std::vector<Room> m_currentOptions;
    bool m_awaitingChoice = true;
    bool m_isShowingDescription = false;
};

} // namespace solis

#endif // EXPLORATION_STATE_H
