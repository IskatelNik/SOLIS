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

/**
 * @brief Инициализация интерфейса исследования.
 */
void ExplorationState::init() {
    sf::Vector2u windowSize = m_window.getSize();
    float w = static_cast<float>(windowSize.x);
    float h = static_cast<float>(windowSize.y);

    const sf::Font& font = ResourceManager::getInstance().getFont("main");

    m_topBar = std::make_unique<UIBox>(sf::Vector2f(0.f, 0.f), sf::Vector2f(w, h * constants::UI_TOPBAR_HEIGHT), constants::COLOR_UI_BG_LIGHT, constants::COLOR_UI_OUTLINE, -2.f);
    m_mainDisplay = std::make_unique<UIBox>(sf::Vector2f(0.f, h * constants::UI_TOPBAR_HEIGHT), sf::Vector2f(w, h * constants::UI_MAIN_DISPLAY_HEIGHT), sf::Color::Black, constants::COLOR_UI_OUTLINE, -2.f);
    m_actionMenu = std::make_unique<UIBox>(sf::Vector2f(0.f, h * (constants::UI_TOPBAR_HEIGHT + constants::UI_MAIN_DISPLAY_HEIGHT)), sf::Vector2f(w, h * constants::UI_ACTION_MENU_HEIGHT), constants::COLOR_UI_BG_DARK, constants::COLOR_UI_OUTLINE, -2.f);

    generateNextStep();
    m_keyHeld = true; // Предотвращение ложного срабатывания ввода при смене состояний
}

/**
 * @brief Генерирует и отображает новые пути для игрока.
 */
void ExplorationState::generateNextStep() {
    m_currentOptions = RunManager::getInstance().getNextRoomOptions();
    const sf::Font& font = ResourceManager::getInstance().getFont("main");
    
    std::string transitionText = "ПЕРЕД ВАМИ НОВЫЕ ПУТИ...\n\nТень отступает, открывая несколько проходов. Куда вы направитесь дальше?";
    m_mainDisplay->setText(sf::String::fromUtf8(transitionText.begin(), transitionText.end()), font, 24, sf::Color::Yellow);

    // Обновление инфо-панели (Локация и Шаг)
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

/**
 * @brief Обработка выбора пути и взаимодействия с объектами в комнатах.
 */
void ExplorationState::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) m_window.close();

    bool anyNumPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3);
    bool spacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (!anyNumPressed && !spacePressed) { m_keyHeld = false; return; }
    if (m_keyHeld) return;

    if (m_awaitingChoice && !m_isShowingDescription) {
        // Логика выбора следующей комнаты
        int choice = -1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) choice = 0;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2)) choice = 1;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3)) choice = 2;

        if (choice >= 0 && static_cast<size_t>(choice) < m_currentOptions.size()) {
            m_keyHeld = true; 
            const Room& selectedRoom = m_currentOptions[choice];
            const sf::Font& font = ResourceManager::getInstance().getFont("main");
            
            std::string finalDesc = selectedRoom.description;
            
            // Окружающий урон в Печи (Уровень 4)
            if (RunManager::getInstance().getCurrentLevel() == 4 && selectedRoom.type != "combat" && selectedRoom.type != "boss") {
                RunManager::getInstance().getPlayer().addHeat(5.0f);
                finalDesc += "\n\n[ВЛИЯНИЕ УРОВНЯ: Окружающая среда обжигает (+5% ЖАРА)]";
            }
            
            m_mainDisplay->setText(sf::String::fromUtf8(finalDesc.begin(), finalDesc.end()), font, 22, sf::Color::White);
            RunManager::getInstance().moveToRoom(selectedRoom);

            // Обработка типа комнаты (Бой, Лор, Отдых, Текстовый квест)
            if (selectedRoom.type == "combat" || selectedRoom.type == "boss") {
                if (!selectedRoom.possible_enemies.empty()) {
                    m_pendingEnemyId = (selectedRoom.type == "boss") ? selectedRoom.possible_enemies[0] : selectedRoom.possible_enemies[rand() % selectedRoom.possible_enemies.size()];
                    std::string prompt = (selectedRoom.type == "boss") ? "[Space] БРОСИТЬ ВЫЗОВ БОССУ" : "[Space] ВСТУПИТЬ В БОЙ";
                    m_actionMenu->setText(sf::String::fromUtf8(prompt.begin(), prompt.end()), font, 24, sf::Color::Red);
                }
            } else if (selectedRoom.type == "lore_room") {
                // Логика обнаружения новых знаний
                const auto& allLore = DataManager::getInstance().getLore();
                std::vector<std::string> available;
                int currentLevel = RunManager::getInstance().getCurrentLevel();
                for (auto const& [id, lore] : allLore) {
                    if (lore.target_trait != -1 && lore.level == currentLevel && !RunManager::getInstance().isLoreUnlocked(id)) {
                        available.push_back(id);
                    }
                }
                if (!available.empty()) {
                    m_discoveredLoreId = available[rand() % available.size()];
                    const auto& lore = allLore.at(m_discoveredLoreId);
                    m_mainDisplay->setText(sf::String::fromUtf8(lore.pickup_text.begin(), lore.pickup_text.end()), font, 22, constants::COLOR_LORE_DISCOVERY);
                    RunManager::getInstance().unlockLore(m_discoveredLoreId);
                } else {
                    m_mainDisplay->setText("АРХИВ ПУСТ\n\nЗдесь больше нечего искать.", font, 22, sf::Color::Green);
                }
                m_actionMenu->setText("[Space] Продолжить...", font, 24, sf::Color::Yellow);
            } else if (selectedRoom.type == "heat_sink") {
                m_pendingTerminalRoom = &selectedRoom;
                m_actionMenu->setText("[1] Использовать терминал\n[2] Пройти мимо", font, 24, sf::Color::Green);
            } else if (selectedRoom.type == "quest") {
                // Логика запуска линейного текстового события
                int currentLevel = RunManager::getInstance().getCurrentLevel();
                int progress = RunManager::getInstance().getEventProgress(currentLevel);
                const auto& allEvents = DataManager::getInstance().getEvents();
                for (const auto& [id, ev] : allEvents) {
                    if (ev.level == currentLevel && ev.order_index == progress) {
                        m_pendingEvent = &ev;
                        break;
                    }
                }
                if (m_pendingEvent) {
                    std::string text = m_pendingEvent->preview_text + "\n\n" + m_pendingEvent->description;
                    m_mainDisplay->setText(sf::String::fromUtf8(text.begin(), text.end()), font, 22, sf::Color::White);
                    std::string choices;
                    for (size_t i = 0; i < m_pendingEvent->choices.size(); ++i) {
                        choices += "[" + std::to_string(i + 1) + "] " + m_pendingEvent->choices[i].text + "\n";
                    }
                    m_actionMenu->setText(sf::String::fromUtf8(choices.begin(), choices.end()), font, 24, sf::Color::Green);
                } else {
                    m_mainDisplay->setText("ПУСТОТА\n\nЗдесь больше нет событий.", font, 22, sf::Color::White);
                    m_actionMenu->setText("[Space] Продолжить...", font, 24, sf::Color::Yellow);
                }
            } else {
                m_actionMenu->setText("[Space] Продолжить...", font, 24, sf::Color::Yellow);
            }

            m_isShowingDescription = true;
            m_awaitingChoice = false;
        }
    } 
    else if (m_isShowingDescription) {
        // Вторичная обработка внутри комнат (выбор в квестах или терминалах)
        if (m_pendingTerminalRoom) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) {
                m_keyHeld = true;
                Player& player = RunManager::getInstance().getPlayer();
                float heatVented = player.getHeat() * constants::TERMINAL_HEAT_VENT_PERCENT;
                int healAmount = (int)(player.getMaxHp() * constants::TERMINAL_HEAL_PERCENT);
                player.reduceHeat(heatVented);
                player.heal(healAmount);
                std::stringstream ss;
                ss << "СБРОШЕНО ЖАРА: " << (int)heatVented << "%\nВОССТАНОВЛЕНО HP: " << healAmount;
                m_mainDisplay->setText(sf::String::fromUtf8(ss.str().begin(), ss.str().end()), ResourceManager::getInstance().getFont("main"), 22, constants::COLOR_LORE_DISCOVERY);
                m_actionMenu->setText("[Space] Продолжить...", ResourceManager::getInstance().getFont("main"), 24, sf::Color::Yellow);
                m_pendingTerminalRoom = nullptr;
            } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2)) {
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
                const auto& choice = m_pendingEvent->choices[evChoice - 1];
                Player& player = RunManager::getInstance().getPlayer();
                if (choice.heat_change > 0) player.addHeat(choice.heat_change);
                else if (choice.heat_change < 0) player.reduceHeat(-choice.heat_change);
                RunManager::getInstance().modifyIdeologyScore(choice.ideology_change);
                RunManager::getInstance().incrementEventProgress(m_pendingEvent->level);
                std::stringstream ss;
                ss << "ВЫБОР СДЕЛАН\n\n";
                if (choice.heat_change != 0) ss << (choice.heat_change > 0 ? "[ЖАР УВЕЛИЧЕН: " : "[ЖАР УМЕНЬШЕН: ") << (int)std::abs(choice.heat_change) << "%]\n";
                if (choice.ideology_change != 0) ss << (choice.ideology_change > 0 ? "[ПУТЬ: СОЛИДАРНОСТЬ]" : "[ПУТЬ: ПРАГМАТИЗМ]") << "\n";
                m_mainDisplay->setText(sf::String::fromUtf8(ss.str().begin(), ss.str().end()), ResourceManager::getInstance().getFont("main"), 22, constants::COLOR_LORE_DISCOVERY);
                m_actionMenu->setText("[Space] Продолжить...", ResourceManager::getInstance().getFont("main"), 24, sf::Color::Yellow);
                m_pendingEvent = nullptr;
            }
        }
        else if (spacePressed) {
            m_keyHeld = true;
            m_isShowingDescription = false;
            if (!m_pendingEnemyId.empty()) {
                auto enemy = (m_pendingEnemyId.find("boss") != std::string::npos) ? DataManager::getInstance().spawnBoss(m_pendingEnemyId) : DataManager::getInstance().spawnEnemy(m_pendingEnemyId);
                m_pendingEnemyId = "";
                if (enemy) m_stateMachine.pushState(std::make_unique<CombatState>(m_window, m_stateMachine, std::move(enemy)));
            } else {
                generateNextStep();
            }
        }
    }
}

void ExplorationState::resume() {
    m_keyHeld = true; 
    generateNextStep();
}

void ExplorationState::update(float deltaTime) {}

void ExplorationState::render(sf::RenderWindow& window) {
    if (m_topBar) m_topBar->render(window);
    if (m_mainDisplay) m_mainDisplay->render(window);
    if (m_actionMenu) m_actionMenu->render(window);
}

} // namespace solis
