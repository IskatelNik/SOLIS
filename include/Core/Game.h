#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include "Core/StateMachine.h"
#include <string>

namespace solis {

class Game {
public:
    Game(const std::string& title, unsigned int width, unsigned int height);
    ~Game() = default;

    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

    sf::RenderWindow m_window;
    StateMachine m_stateMachine;
    
    const float m_deltaTime = 1.0f / 60.0f; // 60 FPS target
};

} // namespace solis

#endif // GAME_H
