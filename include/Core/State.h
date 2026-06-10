#ifndef STATE_H
#define STATE_H

#include <SFML/Graphics.hpp>

namespace solis {

class State {
public:
    virtual ~State() = default;

    virtual void init() = 0;
    virtual void resume() {} // Called when returning to this state after a pop
    virtual void handleInput() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
};

} // namespace solis

#endif // STATE_H
