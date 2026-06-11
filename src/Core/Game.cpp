#include "Core/Game.h"
#include "States/HubState.h"
#include "Core/ResourceManager.h"
#include "Managers/DataManager.h"
#include "Managers/RunManager.h"
#include "Managers/SaveManager.h"
#include "Utils/GameConstans.h"
#include <iostream>

namespace solis {

Game::Game(const std::string& title, unsigned int width, unsigned int height)
    : m_window(sf::VideoMode({width, height}), title) {
    
    // Загружаем шрифт по умолчанию
    if (!ResourceManager::getInstance().loadFont("main", "assets/fonts/font_main.ttf")) {
        std::cerr << "[Game] Warning: Default font not found at assets/fonts/font_main.ttf" << std::endl;
    }

    // Загружаем ASCII шрифт (если не найден, используем main как фоллбэк)
    if (!ResourceManager::getInstance().loadFont("ascii", "assets/fonts/font_ascii.ttf")) {
        std::cerr << "[Game] Warning: ASCII font not found, falling back to main font." << std::endl;
        ResourceManager::getInstance().loadFont("ascii", "assets/fonts/font_main.ttf");
    }

    // Загружаем базы данных
    DataManager::getInstance().loadRooms("data/rooms.json");
    DataManager::getInstance().loadEnemies("data/enemies.json");
    DataManager::getInstance().loadBosses("data/bosses.json");
    DataManager::getInstance().loadSkills("data/skills.json");
    DataManager::getInstance().loadLore("data/lore.json");
    DataManager::getInstance().loadUpgrades("data/upgrades.json");
    DataManager::getInstance().loadArtifacts("data/artifacts.json");
    DataManager::getInstance().loadEvents("data/events.json");

    // Загружаем сохранение (MVP 4)
    if (!SaveManager::getInstance().load()) {
        std::cerr << "[Game] Warning: Could not load or create save file." << std::endl;
    }

    // Устанавливаем начальное состояние - Хаб
    m_stateMachine.pushState(std::make_unique<HubState>(m_window, m_stateMachine));
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
