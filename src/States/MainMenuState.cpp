#include "States/MainMenuState.h"
#include "Core/ResourceManager.h"

namespace solis {

/**
  Начальное состояние главного меню (используется как точка входа или отладка).
 */
MainMenuState::MainMenuState(sf::RenderWindow& window, StateMachine& stateMachine)
    : m_window(window), m_stateMachine(stateMachine) {}

void MainMenuState::init() {
    sf::Vector2u windowSize = m_window.getSize();
    float w = static_cast<float>(windowSize.x);
    float h = static_cast<float>(windowSize.y);

    const sf::Font& font = ResourceManager::getInstance().getFont("main");

    // Инициализация базового макета UI
    m_topBar = std::make_unique<UIBox>(sf::Vector2f(0.f, 0.f), sf::Vector2f(w, h * 0.1f), sf::Color(30, 30, 30), sf::Color::White, -2.f);
    m_topBar->setText("SOLIS", font, 24, sf::Color::Yellow);

    m_mainDisplay = std::make_unique<UIBox>(sf::Vector2f(0.f, h * 0.1f), sf::Vector2f(w, h * 0.55f), sf::Color::Black, sf::Color::White, -2.f);
    m_mainDisplay->setText("Главное Меню - Нажмите [1] для входа в Хаб", font, 20, sf::Color::White);

    m_actionMenu = std::make_unique<UIBox>(sf::Vector2f(0.f, h * 0.65f), sf::Vector2f(w, h * 0.35f), sf::Color(20, 20, 20), sf::Color::White, -2.f);
    m_actionMenu->setText("[1] Войти в игру\n[Esc] Выход", font, 24, sf::Color::Green);
}

void MainMenuState::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) m_window.close();
    
    // В текущей версии игра сразу стартует в HubState, MainMenu оставлен для структуры
}

void MainMenuState::update(float deltaTime) {}

void MainMenuState::render(sf::RenderWindow& window) {
    if (m_topBar) m_topBar->render(window);
    if (m_mainDisplay) m_mainDisplay->render(window);
    if (m_actionMenu) m_actionMenu->render(window);
}

} // namespace solis
