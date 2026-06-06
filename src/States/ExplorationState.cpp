#include "States/ExplorationState.h"
#include "States/CombatState.h"
#include "Core/ResourceManager.h"
#include "Managers/RunManager.h"
#include "Managers/DataManager.h"
#include <string>
#include <random>
#include <sstream>

namespace solis {

ExplorationState::ExplorationState(sf::RenderWindow& window, StateMachine& stateMachine)
    : m_window(window), m_stateMachine(stateMachine) {}

void ExplorationState::init() {
    sf::Vector2u windowSize = m_window.getSize();
    float w = static_cast<float>(windowSize.x);
    float h = static_cast<float>(windowSize.y);

    const sf::Font& font = ResourceManager::getInstance().getFont("main");

    m_topBar = std::make_unique<UIBox>(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(w, h * 0.1f),
        sf::Color(30, 30, 30),
        sf::Color::White,
        -2.f
    );

    m_mainDisplay = std::make_unique<UIBox>(
        sf::Vector2f(0.f, h * 0.1f),
        sf::Vector2f(w, h * 0.55f),
        sf::Color::Black,
        sf::Color::White,
        -2.f
    );

    m_actionMenu = std::make_unique<UIBox>(
        sf::Vector2f(0.f, h * 0.65f),
        sf::Vector2f(w, h * 0.35f),
        sf::Color(20, 20, 20),
        sf::Color::White,
        -2.f
    );

    generateNextStep();
}

void ExplorationState::generateNextStep() {
    m_currentOptions = RunManager::getInstance().getNextRoomOptions();
    
    const sf::Font& font = ResourceManager::getInstance().getFont("main");
    
    std::string transitionText = "ПЕРЕД ВАМИ НОВЫЕ ПУТИ...\n\nТень отступает, открывая несколько проходов. Куда вы направитесь дальше?";
    m_mainDisplay->setText(sf::String::fromUtf8(transitionText.begin(), transitionText.end()), font, 24, sf::Color::Yellow);

    std::string levelInfo = "УРОВЕНЬ: " + std::to_string(RunManager::getInstance().getCurrentLevel()) + 
                            " | ШАГ: " + std::to_string(RunManager::getInstance().getCurrentRoomIndex());
    m_topBar->setText(sf::String::fromUtf8(levelInfo.begin(), levelInfo.end()), font, 24, sf::Color::Yellow);

    std::string menuText;
    for (size_t i = 0; i < m_currentOptions.size(); ++i) {
        menuText += "[" + std::to_string(i + 1) + "] " + m_currentOptions[i].preview_text + "\n";
    }
    m_actionMenu->setText(sf::String::fromUtf8(menuText.begin(), menuText.end()), font, 24, sf::Color::Green);
    
    m_awaitingChoice = true;
}

void ExplorationState::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        m_window.close();
    }

    static bool keyHeld = false;
    bool anyNumPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1) || 
                         sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2) || 
                         sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3);
    bool spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (!anyNumPressed && !spacePressed) {
        keyHeld = false;
        return;
    }

    if (keyHeld) return;

    if (m_awaitingChoice && !m_isShowingDescription) {
        int choice = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) choice = 0;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2)) choice = 1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3)) choice = 2;

        if (choice >= 0 && static_cast<size_t>(choice) < m_currentOptions.size()) {
            keyHeld = true;
            const Room& selectedRoom = m_currentOptions[choice];
            const sf::Font& font = ResourceManager::getInstance().getFont("main");
            
            m_mainDisplay->setText(sf::String::fromUtf8(selectedRoom.description.begin(), selectedRoom.description.end()), font, 22, sf::Color::White);
            RunManager::getInstance().moveToRoom(selectedRoom);

            if (selectedRoom.type == "combat") {
                if (!selectedRoom.possible_enemies.empty()) {
                    static std::random_device rd;
                    static std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, static_cast<int>(selectedRoom.possible_enemies.size()) - 1);
                    m_pendingEnemyId = selectedRoom.possible_enemies[dis(gen)];
                    std::string prompt = "[Space] ВСТУПИТЬ В БОЙ";
                    m_actionMenu->setText(sf::String::fromUtf8(prompt.begin(), prompt.end()), font, 24, sf::Color::Red);
                }
            } else if (selectedRoom.type == "lore_room") {
                m_pendingEnemyId = "";
                const auto& allLore = DataManager::getInstance().getLore();
                std::vector<std::string> available;
                for (auto const& [id, lore] : allLore) {
                    if (lore.target_trait != -1 && !RunManager::getInstance().isLoreUnlocked(id)) {
                        available.push_back(id);
                    }
                }
                if (!available.empty()) {
                    static std::random_device rd;
                    static std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, static_cast<int>(available.size()) - 1);
                    m_discoveredLoreId = available[dis(gen)];
                    const auto& lore = allLore.at(m_discoveredLoreId);
                    m_mainDisplay->setText(sf::String::fromUtf8(lore.pickup_text.begin(), lore.pickup_text.end()), font, 22, sf::Color::Cyan);
                    RunManager::getInstance().unlockLore(m_discoveredLoreId);
                } else {
                    std::string msg = "АРХИВ ПУСТ\n\nЗдесь больше нечего искать.";
                    m_mainDisplay->setText(sf::String::fromUtf8(msg.begin(), msg.end()), font, 22, sf::Color::Green);
                }
                std::string prompt = "[Space] Продолжить...";
                m_actionMenu->setText(sf::String::fromUtf8(prompt.begin(), prompt.end()), font, 24, sf::Color::Yellow);
            } else if (selectedRoom.type == "heat_sink") {
                m_pendingEnemyId = "";
                m_pendingTerminalRoom = &selectedRoom;
                std::string menuChoices = "[1] Использовать терминал\n[2] Пройти мимо";
                m_actionMenu->setText(sf::String::fromUtf8(menuChoices.begin(), menuChoices.end()), font, 24, sf::Color::Green);
            } else {
                m_pendingEnemyId = "";
                std::string prompt = "[Space] Продолжить...";
                m_actionMenu->setText(sf::String::fromUtf8(prompt.begin(), prompt.end()), font, 24, sf::Color::Yellow);
            }

            m_isShowingDescription = true;
            m_awaitingChoice = false;
        }
    } 
    else if (m_isShowingDescription) {
        if (m_pendingTerminalRoom) {
            int termChoice = -1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) termChoice = 1;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2)) termChoice = 2;

            if (termChoice == 1) {
                keyHeld = true;
                Player& player = RunManager::getInstance().getPlayer();
                float heatVented = player.getHeat() * 0.9f;
                int healAmount = (int)(player.getMaxHp() * 0.15f);
                player.reduceHeat(heatVented);
                player.heal(healAmount);

                const sf::Font& font = ResourceManager::getInstance().getFont("main");
                std::stringstream ss;
                ss << "СБРОШЕНО ЖАРА: " << (int)heatVented << "%\nВОССТАНОВЛЕНО HP: " << healAmount;
                std::string msg = ss.str();
                m_mainDisplay->setText(sf::String::fromUtf8(msg.begin(), msg.end()), font, 22, sf::Color::Cyan);
                std::string prompt = "[Space] Продолжить...";
                m_actionMenu->setText(sf::String::fromUtf8(prompt.begin(), prompt.end()), font, 24, sf::Color::Yellow);
                m_pendingTerminalRoom = nullptr;
            } else if (termChoice == 2) {
                keyHeld = true;
                m_pendingTerminalRoom = nullptr;
                m_isShowingDescription = false;
                generateNextStep();
            }
        } 
        else if (spacePressed) {
            keyHeld = true;
            m_isShowingDescription = false;
            m_discoveredLoreId = "";
            if (!m_pendingEnemyId.empty()) {
                auto enemy = DataManager::getInstance().spawnEnemy(m_pendingEnemyId);
                m_pendingEnemyId = "";
                if (enemy) {
                    m_stateMachine.pushState(std::make_unique<CombatState>(m_window, m_stateMachine, std::move(enemy)));
                    generateNextStep(); 
                }
            } else {
                generateNextStep();
            }
        }
    }
}

void ExplorationState::update(float deltaTime) {}

void ExplorationState::render(sf::RenderWindow& window) {
    if (m_topBar) m_topBar->render(window);
    if (m_mainDisplay) m_mainDisplay->render(window);
    if (m_actionMenu) m_actionMenu->render(window);
}

} // namespace solis
