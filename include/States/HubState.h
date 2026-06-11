#ifndef HUB_STATE_H
#define HUB_STATE_H

#include "Core/State.h"
#include "Core/StateMachine.h"
#include "UI/UIBox.h"
#include <memory>

namespace solis {

enum class HubMenu {
    Main,
    Shop,
    Inventory
};

class HubState : public State {
public:
    HubState(sf::RenderWindow& window, StateMachine& stateMachine);

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    void updateUI();
    void handleMainInput(int num);
    void handleShopInput(int num);
    void handleInventoryInput(int num);

    void buyUpgrade(int index);
    void toggleSkill(const std::string& skillId);

    sf::RenderWindow& m_window;
    StateMachine& m_stateMachine;

    std::unique_ptr<UIBox> m_topBar;
    std::unique_ptr<UIBox> m_mainDisplayLeft;
    std::unique_ptr<UIBox> m_mainDisplayRight;
    std::unique_ptr<UIBox> m_actionMenuLeft;
    std::unique_ptr<UIBox> m_actionMenuRight;

    HubMenu m_currentMenu = HubMenu::Main;
    std::vector<std::string> m_shopUpgradeIds;
    bool m_keyHeld = false;
};

} // namespace solis

#endif // HUB_STATE_H
