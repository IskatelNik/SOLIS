#ifndef ENDING_STATE_H
#define ENDING_STATE_H

#include "Core/State.h"
#include "Core/StateMachine.h"
#include "UI/UIBox.h"
#include <memory>

namespace solis {

class EndingState : public State {
public:
    EndingState(sf::RenderWindow& window, StateMachine& stateMachine);

    void init() override;
    void handleInput() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    void calculateEnding();

    sf::RenderWindow& m_window;
    StateMachine& m_stateMachine;

    std::unique_ptr<UIBox> m_display;
    bool m_keyHeld = false;
};

} // namespace solis

#endif // ENDING_STATE_H
