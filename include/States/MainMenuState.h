#ifndef MAIN_MENU_STATE_H
#define MAIN_MENU_STATE_H

#include "Core/State.h"
#include "Core/StateMachine.h"
#include "UI/UIBox.h"
#include <memory>
#include <vector>

namespace solis {

class MainMenuState : public State {
public:
    MainMenuState(sf::RenderWindow& window, StateMachine& stateMachine);

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    sf::RenderWindow& m_window;
    StateMachine& m_stateMachine;

    std::unique_ptr<UIBox> m_topBar;
    std::unique_ptr<UIBox> m_mainDisplay;
    std::unique_ptr<UIBox> m_actionMenu;
};

} // namespace solis

#endif // MAIN_MENU_STATE_H
