#include "Core/Game.h"
#include "States/HubState.h"
#include "Core/ResourceManager.h"
#include "Managers/DataManager.h"
#include "Managers/RunManager.h"
#include "Managers/SaveManager.h"
#include "Utils/GameConstans.h"
#include <iostream>

namespace solis {

/**
 * @brief Конструктор главного класса игры.
 * Инициализирует окно, загружает ресурсы и базы данных, а также устанавливает начальное состояние.
 */
Game::Game(const std::string& title, unsigned int width, unsigned int height)
    : m_window(sf::VideoMode({width, height}), title) {
    
    // Загрузка основного шрифта для интерфейса
    if (!ResourceManager::getInstance().loadFont("main", "assets/fonts/font_main.ttf")) {
        std::cerr << "[Game] Warning: Default font not found at assets/fonts/font_main.ttf" << std::endl;
    }

    // Загрузка всех баз данных из JSON-файлов
    DataManager::getInstance().loadRooms("data/rooms.json");
    DataManager::getInstance().loadEnemies("data/enemies.json");
    DataManager::getInstance().loadBosses("data/bosses.json");
    DataManager::getInstance().loadSkills("data/skills.json");
    DataManager::getInstance().loadLore("data/lore.json");
    DataManager::getInstance().loadUpgrades("data/upgrades.json");
    DataManager::getInstance().loadArtifacts("data/artifacts.json");
    DataManager::getInstance().loadEvents("data/events.json");

    // Загрузка прогресса игрока
    if (!SaveManager::getInstance().load()) {
        std::cerr << "[Game] Warning: Could not load or create save file." << std::endl;
    }

    // Установка начального игрового состояния (Родовой Очаг)
    m_stateMachine.pushState(std::make_unique<HubState>(m_window, m_stateMachine));
}

/**
 * @brief Главный игровой цикл.
 * Использует фиксированный временной шаг (Fixed Timestep) для обновления логики.
 */
void Game::run() {
    sf::Clock clock;
    float accumulator = 0.0f;

    while (m_window.isOpen()) {
        // Применяем изменения состояний (push/pop/change)
        m_stateMachine.processStateChanges();
        
        // Если состояний не осталось — выходим
        if (m_stateMachine.isEmpty()) break;

        float frameTime = clock.restart().asSeconds();
        // Ограничиваем максимальный шаг времени, чтобы избежать скачков при лагах
        if (frameTime > 0.25f) frameTime = 0.25f;

        accumulator += frameTime;

        // Фиксированный шаг обновления логики
        while (accumulator >= m_deltaTime) {
            processEvents();
            m_stateMachine.getActiveState().handleInput();
            m_stateMachine.getActiveState().update(m_deltaTime);
            accumulator -= m_deltaTime;
        }

        // Отрисовка текущего состояния
        render();
    }
}

/**
 * @brief Обработка системных событий окна.
 */
void Game::processEvents() {
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
    }
}

/**
 * @brief Очистка окна и рендеринг активного состояния.
 */
void Game::render() {
    m_window.clear(sf::Color::Black);
    if (!m_stateMachine.isEmpty()) {
        m_stateMachine.getActiveState().render(m_window);
    }
    m_window.display();
}

} // namespace solis
