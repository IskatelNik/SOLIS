#include "Core/Game.h"
#include "States/MainMenuState.h"
#include "Core/ResourceManager.h"
#include <iostream>

namespace solis {

Game::Game(const std::string& title, unsigned int width, unsigned int height)
    : m_window(sf::VideoMode({width, height}), title) {
    
    // Загружаем шрифт по умолчанию
    if (!ResourceManager::getInstance().loadFont("main", "assets/fonts/font_main.ttf")) {
        // Если шрифта нет, MVP 0 может не отрисовать текст, но не должна падать.
        std::cerr << "[Game] Warning: Default font not found at assets/fonts/font_main.ttf" << std::endl;
    }

    // Устанавливаем начальное состояние
    m_stateMachine.pushState(std::make_unique<MainMenuState>(m_window, m_stateMachine));
}

void Game::run() {
    sf::Clock clock;
    float accumulator = 0.0f;

    while (m_window.isOpen()) {
        m_stateMachine.processStateChanges();
        if (m_stateMachine.isEmpty()) break;

        float frameTime = clock.restart().asSeconds();
        if (frameTime > 0.25f) frameTime = 0.25f; // Cap for spikes

        accumulator += frameTime;

        while (accumulator >= m_deltaTime) {
            processEvents();
            m_stateMachine.getActiveState().handleInput();
            m_stateMachine.getActiveState().update(m_deltaTime);
            accumulator -= m_deltaTime;
        }

        render();
    }
}

void Game::processEvents() {
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
    }
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    if (!m_stateMachine.isEmpty()) {
        m_stateMachine.getActiveState().render(m_window);
    }
    m_window.display();
}

} // namespace solis
