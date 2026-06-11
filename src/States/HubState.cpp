#include "States/HubState.h"
#include "States/ExplorationState.h"
#include "Core/ResourceManager.h"
#include "Managers/RunManager.h"
#include "Managers/SaveManager.h"
#include "Managers/DataManager.h"
#include "Utils/GameConstans.h"
#include <sstream>
#include <algorithm>
#include <fstream>

namespace solis {

HubState::HubState(sf::RenderWindow& window, StateMachine& stateMachine)
    : m_window(window), m_stateMachine(stateMachine) {}

void HubState::init() {
    sf::Vector2u windowSize = m_window.getSize();
    float w = static_cast<float>(windowSize.x);
    float h = static_cast<float>(windowSize.y);

    const sf::Font& font = ResourceManager::getInstance().getFont("main");

    m_topBar = std::make_unique<UIBox>(sf::Vector2f(0,0), sf::Vector2f(w, h * constants::UI_TOPBAR_HEIGHT), constants::COLOR_UI_BG_LIGHT, constants::COLOR_UI_OUTLINE, -2.f);
    
    // Main Display: 55% height. Left: 70% width (Log/Dialogue), Right: 30% width (Visual)
    float mainH = h * constants::UI_MAIN_DISPLAY_HEIGHT;
    float mainY = h * constants::UI_TOPBAR_HEIGHT;
    m_mainDisplayLeft = std::make_unique<UIBox>(sf::Vector2f(0, mainY), sf::Vector2f(w * 0.70f, mainH), constants::COLOR_UI_BG_DARK, constants::COLOR_UI_OUTLINE, -2.f);
    m_mainDisplayRight = std::make_unique<UIBox>(sf::Vector2f(w * 0.70f, mainY), sf::Vector2f(w * 0.30f, mainH), constants::COLOR_UI_BG_DARK, constants::COLOR_UI_OUTLINE, -2.f);

    // Action Menu: 35% height. Left: 70% width (Buttons), Right: 30% width (Player Status)
    float actionH = h * constants::UI_ACTION_MENU_HEIGHT;
    float actionY = mainY + mainH;
    m_actionMenuLeft = std::make_unique<UIBox>(sf::Vector2f(0, actionY), sf::Vector2f(w * 0.70f, actionH), constants::COLOR_UI_BG_LIGHT, constants::COLOR_UI_OUTLINE, -2.f);
    m_actionMenuRight = std::make_unique<UIBox>(sf::Vector2f(w * 0.70f, actionY), sf::Vector2f(w * 0.30f, actionH), constants::COLOR_UI_BG_LIGHT, constants::COLOR_UI_OUTLINE, -2.f);

    m_currentMenu = HubMenu::Main;
    updateUI();
}

void HubState::updateUI() {
    const sf::Font& font = ResourceManager::getInstance().getFont("main");
    const sf::Font& fontAscii = ResourceManager::getInstance().getFont("ascii");
    SaveData& save = SaveManager::getInstance().getData();

    // Загрузка ASCII-арта
    std::string asciiArt;
    std::ifstream asciiFile("data/ascii/hearth.txt");
    if (asciiFile.is_open()) {
        std::stringstream buffer;
        buffer << asciiFile.rdbuf();
        asciiArt = buffer.str();
    } else {
        asciiArt = "\n\n  [ASCII АРТ НЕ НАЙДЕН]\n  Положите hearth.txt\n  в data/ascii/";
    }

    // TopBar
    std::stringstream sparksSS;
    sparksSS << "РОДОВОЙ ОЧАГ | ИСКРЫ: " << save.solis_sparks << " | ЭМПАТИЯ: " << save.empathy_level;
    std::string sparksText = sparksSS.str();
    m_topBar->setText(sf::String::fromUtf8(sparksText.begin(), sparksText.end()), font, 24, sf::Color::Yellow);

    std::stringstream mainLeft;
    std::stringstream mainRight;
    std::stringstream actionLeft;
    std::stringstream actionRight;

    int maxHp = constants::PLAYER_DEFAULT_MAX_HP + (std::count(save.unlocked_upgrades.begin(), save.unlocked_upgrades.end(), "upgrade_hp_1") * 20);

    if (m_currentMenu == HubMenu::Main) {
        mainLeft << "Древнее пламя мерцает в центре зала. Вы чувствуете покой и силу предков.\n\n";
        mainLeft << "ТЕКУЩИЙ СТАТУС:\n";
        mainLeft << "- Макс. HP: " << maxHp << "\n";
        mainLeft << "- Экипировано навыков: " << save.equipped_skills.size() << "/3\n";
        mainLeft << "- Знаний в архиве: " << save.unlocked_lore.size();
        
        mainRight << "\n\n" << asciiArt;
        
        actionLeft << "[1] НАЧАТЬ ЗАБЕГ\n";
        actionLeft << "[2] МАГАЗИН УЛУЧШЕНИЙ\n";
        actionLeft << "[3] ИНВЕНТАРЬ (НАВЫКИ)\n";
        actionLeft << "[Esc] Выход";
    } 
    else if (m_currentMenu == HubMenu::Shop) {
        mainLeft << "МАГАЗИН УЛУЧШЕНИЙ\nТратьте Искры Солис, чтобы усилить свою оболочку.\n\n";
        
        m_shopUpgradeIds.clear();
        const auto& allUpgrades = DataManager::getInstance().getUpgrades();
        for (auto const& [id, upg] : allUpgrades) {
            if (std::find(save.unlocked_upgrades.begin(), save.unlocked_upgrades.end(), id) == save.unlocked_upgrades.end()) {
                m_shopUpgradeIds.push_back(id);
            }
        }

        if (m_shopUpgradeIds.empty()) {
            mainLeft << "Все доступные улучшения приобретены!";
        } else {
            for (size_t i = 0; i < m_shopUpgradeIds.size(); ++i) {
                const auto& upg = allUpgrades.at(m_shopUpgradeIds[i]);
                actionLeft << "[" << (i+1) << "] " << upg.name << " (" << upg.cost << ") - " << upg.description << "\n";
            }
        }
        actionLeft << "\n[0] Назад";
    }
    else if (m_currentMenu == HubMenu::Inventory) {
        mainLeft << "ИНВЕНТАРЬ НАВЫКОВ\nВыберите до 3-х активных навыков для следующего забега.\n\n";
        
        mainLeft << "ЭКИПИРОВАНО:\n";
        if (save.equipped_skills.empty()) mainLeft << "- Нет\n";
        for (const auto& sid : save.equipped_skills) {
            if (DataManager::getInstance().getSkills().count(sid)) {
                mainLeft << "> " << DataManager::getInstance().getSkills().at(sid).name << "\n";
            }
        }

        mainLeft << "\nДОСТУПНЫЕ НАВЫКИ:\n";
        std::vector<std::string> available;
        available.push_back("skill_solar_flare");
        for (const auto& uid : save.unlocked_upgrades) {
            if (DataManager::getInstance().getUpgrades().count(uid)) {
                if (DataManager::getInstance().getUpgrades().at(uid).type == "active_skill") {
                    available.push_back(uid);
                }
            }
        }

        for (size_t i = 0; i < available.size(); ++i) {
            const auto& skill = DataManager::getInstance().getSkills().at(available[i]);
            bool isEquipped = std::find(save.equipped_skills.begin(), save.equipped_skills.end(), available[i]) != save.equipped_skills.end();
            actionLeft << "[" << (i+1) << "] " << (isEquipped ? "[V] " : "[ ] ") << skill.name << "\n";
        }
        actionLeft << "\n[0] Назад";
    }

    actionRight << "СТАТУС:\n";
    actionRight << "HP: " << maxHp << "/" << maxHp << "\n";
    actionRight << "HEAT: 0%\n";
    m_actionMenuRight->setProgressBar(0.0f, constants::COLOR_HEAT_NORMAL);

    std::string mlStr = mainLeft.str();
    std::string mrStr = mainRight.str();
    std::string alStr = actionLeft.str();
    std::string arStr = actionRight.str();

    m_mainDisplayLeft->setText(sf::String::fromUtf8(mlStr.begin(), mlStr.end()), font, 22, sf::Color::White);
    m_mainDisplayRight->setText(sf::String::fromUtf8(mrStr.begin(), mrStr.end()), fontAscii, 22, sf::Color(200, 100, 50));
    m_actionMenuLeft->setText(sf::String::fromUtf8(alStr.begin(), alStr.end()), font, 20, sf::Color::Green);
    m_actionMenuRight->setText(sf::String::fromUtf8(arStr.begin(), arStr.end()), font, 20, sf::Color::White);
}

void HubState::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        if (m_currentMenu == HubMenu::Main) m_window.close();
        else { m_currentMenu = HubMenu::Main; m_keyHeld = true; updateUI(); return; }
    }

    bool anyNum = false;
    int num = -1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) { anyNum = true; num = 1; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2)) { anyNum = true; num = 2; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3)) { anyNum = true; num = 3; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4)) { anyNum = true; num = 4; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num5)) { anyNum = true; num = 5; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num6)) { anyNum = true; num = 6; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num7)) { anyNum = true; num = 7; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num8)) { anyNum = true; num = 8; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num9)) { anyNum = true; num = 9; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num0)) { anyNum = true; num = 0; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) { anyNum = true; num = 13; }

    if (!anyNum) { m_keyHeld = false; return; }
    if (m_keyHeld) return;
    m_keyHeld = true;

    if (m_currentMenu == HubMenu::Main) {
        if (num == 1) {
            RunManager::getInstance().startNewRun();
            m_stateMachine.changeState(std::make_unique<ExplorationState>(m_window, m_stateMachine));
        } else if (num == 2) {
            m_currentMenu = HubMenu::Shop;
        } else if (num == 3) {
            m_currentMenu = HubMenu::Inventory;
        }
    } else if (m_currentMenu == HubMenu::Shop) {
        if (num == 0) m_currentMenu = HubMenu::Main;
        else if (num > 0 && (size_t)num <= m_shopUpgradeIds.size()) buyUpgrade(num - 1);
    } else if (m_currentMenu == HubMenu::Inventory) {
        if (num == 0) m_currentMenu = HubMenu::Main;
        else {
             std::vector<std::string> available;
             available.push_back("skill_solar_flare");
             for (const auto& uid : SaveManager::getInstance().getData().unlocked_upgrades) {
                if (DataManager::getInstance().getUpgrades().count(uid) && DataManager::getInstance().getUpgrades().at(uid).type == "active_skill")
                    available.push_back(uid);
             }
             if (num > 0 && (size_t)num <= available.size()) {
                 toggleSkill(available[num-1]);
             }
        }
    }

    updateUI();
}

void HubState::buyUpgrade(int index) {
    if (index >= m_shopUpgradeIds.size()) return;
    
    SaveData& save = SaveManager::getInstance().getData();
    const auto& upg = DataManager::getInstance().getUpgrades().at(m_shopUpgradeIds[index]);
    
    if (save.solis_sparks >= upg.cost) {
        save.solis_sparks -= upg.cost;
        save.unlocked_upgrades.push_back(upg.id);
        SaveManager::getInstance().save();
        updateUI();
    }
}

void HubState::toggleSkill(const std::string& skillId) {
    SaveData& save = SaveManager::getInstance().getData();
    auto it = std::find(save.equipped_skills.begin(), save.equipped_skills.end(), skillId);
    
    if (it != save.equipped_skills.end()) {
        save.equipped_skills.erase(it);
    } else {
        if (save.equipped_skills.size() < 3) {
            save.equipped_skills.push_back(skillId);
        }
    }
    SaveManager::getInstance().save();
}

void HubState::update(float deltaTime) {}

void HubState::render(sf::RenderWindow& window) {
    m_topBar->render(window);
    m_mainDisplayLeft->render(window);
    m_mainDisplayRight->render(window);
    m_actionMenuLeft->render(window);
    m_actionMenuRight->render(window);
}

} // namespace solis