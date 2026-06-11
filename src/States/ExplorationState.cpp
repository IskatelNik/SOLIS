#include "States/ExplorationState.h"
#include "States/CombatState.h"
#include "Core/ResourceManager.h"
#include "Managers/RunManager.h"
#include "Managers/DataManager.h"
#include "Utils/GameConstans.h"
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
        sf::Vector2f(w, h * constants::UI_TOPBAR_HEIGHT),
        constants::COLOR_UI_BG_LIGHT,
        constants::COLOR_UI_OUTLINE,
        -2.f
    );

    m_mainDisplay = std::make_unique<UIBox>(
        sf::Vector2f(0.f, h * constants::UI_TOPBAR_HEIGHT),
        sf::Vector2f(w, h * constants::UI_MAIN_DISPLAY_HEIGHT),
        sf::Color::Black,
        constants::COLOR_UI_OUTLINE,
        -2.f
    );

    m_actionMenu = std::make_unique<UIBox>(
        sf::Vector2f(0.f, h * (constants::UI_TOPBAR_HEIGHT + constants::UI_MAIN_DISPLAY_HEIGHT)),
        sf::Vector2f(w, h * constants::UI_ACTION_MENU_HEIGHT),
        constants::COLOR_UI_BG_DARK,
        constants::COLOR_UI_OUTLINE,
        -2.f
    );

    generateNextStep();
    m_keyHeld = true; // MVP 4 Fix: Prevent input bleed from HubState
}

void ExplorationState::generateNextStep() {
    m_currentOptions = RunManager::getInstance().getNextRoomOptions();
    
    const sf::Font& font = ResourceManager::getInstance().getFont("main");
    
    std::string transitionText = "ПЕРЕД ВАМИ НОВЫЕ ПУТИ...\n\nТень отступает, открывая несколько проходов. Куда вы направитесь дальше?";
    m_mainDisplay->setText(sf::String::fromUtf8(transitionText.begin(), transitionText.end()), font, 24, sf::Color::Yellow);

    int lvl = RunManager::getInstance().getCurrentLevel();
    std::string locName = (lvl == 1) ? "Тюрьма" : (lvl == 2) ? "Мануфактуры" : (lvl == 3) ? "Лаборатории" : "Печь";
    std::string levelInfo = "ЛОКАЦИЯ: " + locName + " | ШАГ: " + std::to_string(RunManager::getInstance().getCurrentRoomIndex());
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

    bool anyNumPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1) || 
                         sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2) || 
                         sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3);
    bool spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
    bool enterPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter);

    // Ждем, пока все управляющие клавиши будут отпущены
    if (!anyNumPressed && !spacePressed && !enterPressed) {
        m_keyHeld = false;
        return;
    }

    // Если любая из клавиш всё еще зажата с прошлого состояния - игнорируем
    if (m_keyHeld) return;

    if (m_awaitingChoice && !m_isShowingDescription) {
        int choice = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) choice = 0;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2)) choice = 1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3)) choice = 2;

        if (choice >= 0 && static_cast<size_t>(choice) < m_currentOptions.size()) {
            m_keyHeld = true; // Используем переменную класса
            const Room& selectedRoom = m_currentOptions[choice];
            const sf::Font& font = ResourceManager::getInstance().getFont("main");
            
            std::string finalDesc = selectedRoom.description;
            
            // MVP 5 Biome 4 Update: +5% heat on entering non-combat room
            if (RunManager::getInstance().getCurrentLevel() == 4 && selectedRoom.type != "combat" && selectedRoom.type != "boss") {
                RunManager::getInstance().getPlayer().addHeat(5.0f);
                finalDesc += "\n\n[ВЛИЯНИЕ УРОВНЯ: Окружающая среда обжигает (+5% ЖАРА)]";
            }
            
            m_mainDisplay->setText(sf::String::fromUtf8(finalDesc.begin(), finalDesc.end()), font, 22, sf::Color::White);
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
            } else if (selectedRoom.type == "boss") {
                if (!selectedRoom.possible_enemies.empty()) {
                    m_pendingEnemyId = selectedRoom.possible_enemies[0]; // Первого босса из списка
                    std::string prompt = "[Space] БРОСИТЬ ВЫЗОВ БОССУ";
                    m_actionMenu->setText(sf::String::fromUtf8(prompt.begin(), prompt.end()), font, 24, sf::Color::Red);
                }
            } else if (selectedRoom.type == "lore_room") {
                m_pendingEnemyId = "";
                const auto& allLore = DataManager::getInstance().getLore();
                std::vector<std::string> available;
                int currentLevel = RunManager::getInstance().getCurrentLevel();
                for (auto const& [id, lore] : allLore) {
                    if (lore.target_trait != -1 && lore.level == currentLevel && !RunManager::getInstance().isLoreUnlocked(id)) {
                        available.push_back(id);
                    }
                }
                if (!available.empty()) {
                    static std::random_device rd;
                    static std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, static_cast<int>(available.size()) - 1);
                    m_discoveredLoreId = available[dis(gen)];
                    const auto& lore = allLore.at(m_discoveredLoreId);
                    m_mainDisplay->setText(sf::String::fromUtf8(lore.pickup_text.begin(), lore.pickup_text.end()), font, 22, constants::COLOR_LORE_DISCOVERY);
                    RunManager::getInstance().unlockLore(m_discoveredLoreId);
                } else {
                    std::string msg = "АРХИВ ПУСТ\n\nЗдесь больше нечего искать.";
                    m_mainDisplay->setText(sf::String::fromUtf8(msg.begin(), msg.end()), font, 22, sf::Color::Green);
                }
                std::string prompt = "[Space] Продолжить...";
                m_actionMenu->setText(sf::String::fromUtf8(prompt.begin(), prompt.end()), font, 24, sf::Color::Yellow);
            } else if (selectedRoom.type == "heat_sink") {
                m_pendingEnemyId = "";
                m_pendingEvent = nullptr;
                m_pendingTerminalRoom = &selectedRoom;
                std::string menuChoices = "[1] Использовать терминал\n[2] Пройти мимо";
                m_actionMenu->setText(sf::String::fromUtf8(menuChoices.begin(), menuChoices.end()), font, 24, sf::Color::Green);
            } else if (selectedRoom.type == "quest") {
                m_pendingEnemyId = "";
                m_pendingTerminalRoom = nullptr;
                m_discoveredLoreId = "";

                int currentLevel = RunManager::getInstance().getCurrentLevel();
                int progress = RunManager::getInstance().getEventProgress(currentLevel);
                
                const auto& allEvents = DataManager::getInstance().getEvents();
                const Event* currentEvent = nullptr;
                for (const auto& [id, ev] : allEvents) {
                    if (ev.level == currentLevel && ev.order_index == progress) {
                        currentEvent = &ev;
                        break;
                    }
                }
                
                if (currentEvent) {
                    m_pendingEvent = currentEvent;
                    std::string text = currentEvent->preview_text + "\n\n" + currentEvent->description;
                    m_mainDisplay->setText(sf::String::fromUtf8(text.begin(), text.end()), font, 22, sf::Color::White);
                    
                    std::string menuChoices;
                    for (size_t i = 0; i < currentEvent->choices.size(); ++i) {
                        menuChoices += "[" + std::to_string(i + 1) + "] " + currentEvent->choices[i].text + (i < currentEvent->choices.size() - 1 ? "\n" : "");
                    }
                    m_actionMenu->setText(sf::String::fromUtf8(menuChoices.begin(), menuChoices.end()), font, 24, sf::Color::Green);
                } else {
                    std::string msg = "ПУСТОТА\n\nЗдесь больше нет событий.";
                    m_mainDisplay->setText(sf::String::fromUtf8(msg.begin(), msg.end()), font, 22, sf::Color::White);
                    std::string prompt = "[Space] Продолжить...";
                    m_actionMenu->setText(sf::String::fromUtf8(prompt.begin(), prompt.end()), font, 24, sf::Color::Yellow);
                }
            } else {
                m_pendingEnemyId = "";
                m_pendingEvent = nullptr;
                m_pendingTerminalRoom = nullptr;
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
                m_keyHeld = true; // Используем переменную класса
                Player& player = RunManager::getInstance().getPlayer();
                float heatVented = player.getHeat() * constants::TERMINAL_HEAT_VENT_PERCENT;
                int healAmount = (int)(player.getMaxHp() * constants::TERMINAL_HEAL_PERCENT);
                player.reduceHeat(heatVented);
                player.heal(healAmount);

                const sf::Font& font = ResourceManager::getInstance().getFont("main");
                std::stringstream ss;
                ss << "СБРОШЕНО ЖАРА: " << (int)heatVented << "%\nВОССТАНОВЛЕНО HP: " << healAmount;
                std::string msg = ss.str();
                m_mainDisplay->setText(sf::String::fromUtf8(msg.begin(), msg.end()), font, 22, constants::COLOR_LORE_DISCOVERY);
                std::string prompt = "[Space] Продолжить...";
                m_actionMenu->setText(sf::String::fromUtf8(prompt.begin(), prompt.end()), font, 24, sf::Color::Yellow);
                m_pendingTerminalRoom = nullptr;
            } else if (termChoice == 2) {
                m_keyHeld = true;
                m_pendingTerminalRoom = nullptr;
                m_isShowingDescription = false;
                generateNextStep();
            }
        } else if (m_pendingEvent) {
            int evChoice = -1;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) evChoice = 1;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2)) evChoice = 2;
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3)) evChoice = 3;

            if (evChoice >= 1 && evChoice <= static_cast<int>(m_pendingEvent->choices.size())) {
                m_keyHeld = true;
                const EventChoice& choice = m_pendingEvent->choices[evChoice - 1];
                
                Player& player = RunManager::getInstance().getPlayer();
                if (choice.heat_change > 0) player.addHeat(choice.heat_change);
                else if (choice.heat_change < 0) player.reduceHeat(-choice.heat_change);
                
                RunManager::getInstance().modifyIdeologyScore(choice.ideology_change);
                RunManager::getInstance().incrementEventProgress(m_pendingEvent->level);
                
                const sf::Font& font = ResourceManager::getInstance().getFont("main");
                
                std::stringstream ss;
                ss << "ВЫБОР СДЕЛАН\n\n";
                if (choice.heat_change != 0) {
                    ss << (choice.heat_change > 0 ? "[ЖАР УВЕЛИЧЕН: " : "[ЖАР УМЕНЬШЕН: ") << (int)std::abs(choice.heat_change) << "%]\n";
                }
                if (choice.ideology_change != 0) {
                    ss << (choice.ideology_change > 0 ? "[ПУТЬ: СОЛИДАРНОСТЬ]" : "[ПУТЬ: ПРАГМАТИЗМ]") << "\n";
                }
                std::string result = ss.str();

                m_mainDisplay->setText(sf::String::fromUtf8(result.begin(), result.end()), font, 22, constants::COLOR_LORE_DISCOVERY);
                std::string prompt = "[Space] Продолжить...";
                m_actionMenu->setText(sf::String::fromUtf8(prompt.begin(), prompt.end()), font, 24, sf::Color::Yellow);
                m_pendingEvent = nullptr;
            }
        }
        else if (spacePressed) {
            m_keyHeld = true;
            m_isShowingDescription = false;
            m_discoveredLoreId = "";
            if (!m_pendingEnemyId.empty()) {
                std::unique_ptr<Enemy> enemy;
                
                // Проверяем, босс ли это (костыль, но для MVP работает)
                if (m_pendingEnemyId.find("boss") != std::string::npos) {
                    enemy = DataManager::getInstance().spawnBoss(m_pendingEnemyId);
                } else {
                    enemy = DataManager::getInstance().spawnEnemy(m_pendingEnemyId);
                }

                m_pendingEnemyId = "";
                if (enemy) {
                    m_stateMachine.pushState(std::make_unique<CombatState>(m_window, m_stateMachine, std::move(enemy)));
                    // Мы не генерируем шаги здесь. Это сделает resume(), когда CombatState завершится.
                }
            } else {
                generateNextStep();
            }
        }
    }
}

void ExplorationState::resume() {
    m_keyHeld = true; // Защита от случайных прокликиваний
    generateNextStep();
}

void ExplorationState::update(float deltaTime) {}

void ExplorationState::render(sf::RenderWindow& window) {
    if (m_topBar) m_topBar->render(window);
    if (m_mainDisplay) m_mainDisplay->render(window);
    if (m_actionMenu) m_actionMenu->render(window);
}

} // namespace solis
