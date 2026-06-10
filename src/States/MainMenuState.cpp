#include "States/MainMenuState.h"
#include "Core/ResourceManager.h"
#include <iostream>

namespace solis {

MainMenuState::MainMenuState(sf::RenderWindow& window, StateMachine& stateMachine)
    : m_window(window), m_stateMachine(stateMachine) {}

void MainMenuState::init() {
    sf::Vector2u windowSize = m_window.getSize();
    float w = static_cast<float>(windowSize.x);
    float h = static_cast<float>(windowSize.y);

    const sf::Font& font = ResourceManager::getInstance().getFont("main");

    // TopBar: 10% высоты
    m_topBar = std::make_unique<UIBox>(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(w, h * 0.1f),
        sf::Color(30, 30, 30),
        sf::Color::White,
        -2.f
    );
    
    std::string topBarText = "SOLIS: MVP 0 | HP: 100/100 | HEAT: 0%";
    m_topBar->setText(sf::String::fromUtf8(topBarText.begin(), topBarText.end()), font, 24, sf::Color::Yellow);

    // MainDisplay: 55% высоты
    m_mainDisplay = std::make_unique<UIBox>(
        sf::Vector2f(0.f, h * 0.1f),
        sf::Vector2f(w, h * 0.55f),
        sf::Color::Black,
        sf::Color::White,
        -2.f
    );
    std::string testText = "TESTING WORD WRAP SYSTEM: This is a very long string that should be automatically wrapped by the UIBox component. If the logic works correctly, the text will not overflow the boundaries of this box and instead will continue on the next line. SOLIS uses a modular UI architecture for maximum flexibility.";
    m_mainDisplay->setText(sf::String::fromUtf8(testText.begin(), testText.end()), font, 20, sf::Color::White);

    // ActionMenu: 35% высоты
    m_actionMenu = std::make_unique<UIBox>(
        sf::Vector2f(0.f, h * 0.65f),
        sf::Vector2f(w, h * 0.35f),
        sf::Color(20, 20, 20),
        sf::Color::White,
        -2.f
    );
    std::string actionText = "[1] Начать игру (Not active)\n[2] Выход (Esc)";
    m_actionMenu->setText(sf::String::fromUtf8(actionText.begin(), actionText.end()), font, 24, sf::Color::Green);
}

void MainMenuState::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        m_window.close();
    }
}

void MainMenuState::update(float deltaTime) {
    // В MVP 0 логика обновления минимальна
}

void MainMenuState::render(sf::RenderWindow& window) {
    if (m_topBar) m_topBar->render(window);
    if (m_mainDisplay) m_mainDisplay->render(window);
    if (m_actionMenu) m_actionMenu->render(window);
}

} // namespace solis
