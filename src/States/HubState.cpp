#include "States/HubState.h"
#include "States/ExplorationState.h"
#include "Core/ResourceManager.h"
#include "Managers/RunManager.h"
#include "Managers/SaveManager.h"
#include "Managers/DataManager.h"
#include "Utils/GameConstans.h"
#include <sstream>
#include <algorithm>

namespace solis {

/**
 * @brief Состояние "Родовой Очаг" (Хаб). Место между забегами для прокачки и выбора навыков.
 */
HubState::HubState(sf::RenderWindow& window, StateMachine& stateMachine)
    : m_window(window), m_stateMachine(stateMachine) {}

/**
 * @brief Инициализация интерфейса Хаба.
 */
void HubState::init() {
    sf::Vector2u windowSize = m_window.getSize();
    float w = static_cast<float>(windowSize.x);
    float h = static_cast<float>(windowSize.y);

    const sf::Font& font = ResourceManager::getInstance().getFont("main");

    m_topBar = std::make_unique<UIBox>(sf::Vector2f(0,0), sf::Vector2f(w, h * constants::UI_TOPBAR_HEIGHT), constants::COLOR_UI_BG_LIGHT, constants::COLOR_UI_OUTLINE, -2.f);
    m_mainDisplay = std::make_unique<UIBox>(sf::Vector2f(0, h * constants::UI_TOPBAR_HEIGHT), sf::Vector2f(w, h * constants::UI_MAIN_DISPLAY_HEIGHT), sf::Color::Black, constants::COLOR_UI_OUTLINE, -2.f);
    m_actionMenu = std::make_unique<UIBox>(sf::Vector2f(0, h * (constants::UI_TOPBAR_HEIGHT + constants::UI_MAIN_DISPLAY_HEIGHT)), sf::Vector2f(w, h * constants::UI_ACTION_MENU_HEIGHT), constants::COLOR_UI_BG_DARK, constants::COLOR_UI_OUTLINE, -2.f);

    m_currentMenu = HubMenu::Main;
    updateUI();
}

/**
 * @brief Перерисовывает текстовый контент Хаба в зависимости от выбранного подменю.
 */
void HubState::updateUI() {
    const sf::Font& font = ResourceManager::getInstance().getFont("main");
    SaveData& save = SaveManager::getInstance().getData();

    // Верхняя панель: валюта и эмпатия
    std::stringstream sparksSS;
    sparksSS << "РОДОВОЙ ОЧАГ | ИСКРЫ: " << save.solis_sparks;
    m_topBar->setText(sf::String::fromUtf8(sparksSS.str().begin(), sparksSS.str().end()), font, 24, sf::Color::Yellow);

    std::stringstream ss; // Основной текст
    std::stringstream as; // Текст меню действий

    if (m_currentMenu == HubMenu::Main) {
        ss << "Одинокая искра разгорается в блеклой камере, Солис взывает к своему аватару... Что сделаешь ты на этот раз?\n\n";
        ss << "ТЕКУЩИЙ СТАТУС:\n";
        
        // Расчет бонусного здоровья от пассивок
        int hpBonus = 0;
        const auto& upgradeDb = DataManager::getInstance().getUpgrades();
        for (const auto& uid : save.unlocked_upgrades) {
            if (upgradeDb.count(uid)) {
                const auto& u = upgradeDb.at(uid);
                if (u.type == "passive_stat" && u.effect_target == "max_hp") hpBonus += (int)u.effect_value;
            }
        }

        ss << "- Макс. HP: " << (constants::PLAYER_DEFAULT_MAX_HP + hpBonus) << "\n";
        ss << "- Экипировано навыков: " << save.equipped_skills.size() << "/3\n";
        ss << "- Знаний в архиве: " << save.unlocked_lore.size();
        
        as << "[1] НАЧАТЬ ЗАБЕГ\n";
        as << "[2] МАГАЗИН УЛУЧШЕНИЙ\n";
        as << "[3] ИНВЕНТАРЬ (НАВЫКИ)\n";
        as << "[Esc] Выход";
    } 
    else if (m_currentMenu == HubMenu::Shop) {
        ss << "МАГАЗИН УЛУЧШЕНИЙ\nТратьте Искры Солис, чтобы усилить свою оболочку.\n\n";
        
        // Формирование списка доступных для покупки улучшений
        m_shopUpgradeIds.clear();
        const auto& allUpgrades = DataManager::getInstance().getUpgrades();
        for (auto const& [id, upg] : allUpgrades) {
            if (std::find(save.unlocked_upgrades.begin(), save.unlocked_upgrades.end(), id) == save.unlocked_upgrades.end()) {
                m_shopUpgradeIds.push_back(id);
            }
        }

        if (m_shopUpgradeIds.empty()) {
            ss << "Все доступные улучшения приобретены!";
        } else {
            // Постраничный вывод (по 5 штук)
            size_t pageSize = 5;
            size_t start = m_shopPage * pageSize;
            size_t end = std::min(start + pageSize, m_shopUpgradeIds.size());
            int maxPage = (int)((m_shopUpgradeIds.size() - 1) / pageSize);

            if (m_shopPage > maxPage) m_shopPage = maxPage;

            for (size_t i = start; i < end; ++i) {
                const auto& upg = allUpgrades.at(m_shopUpgradeIds[i]);
                as << "[" << (i - start + 1) << "] " << upg.name << " (" << upg.cost << ")\n    " << upg.description << "\n";
            }

            as << "\n";
            if (m_shopPage > 0) as << "[8] Пред. страница ";
            if (end < m_shopUpgradeIds.size()) as << "[9] След. страница";
            as << "\nСтраница " << (m_shopPage + 1) << " из " << (maxPage + 1);
        }
        as << "\n[0] Назад";
    }
    else if (m_currentMenu == HubMenu::Inventory) {
        ss << "ИНВЕНТАРЬ НАВЫКОВ\nВыберите до 3-х активных навыков для следующего забега.\n\n";
        
        ss << "ЭКИПИРОВАНО:\n";
        if (save.equipped_skills.empty()) ss << "- Нет\n";
        for (const auto& sid : save.equipped_skills) {
            if (DataManager::getInstance().getSkills().count(sid)) {
                ss << "> " << DataManager::getInstance().getSkills().at(sid).name << "\n";
            }
        }

        // Формирование списка разблокированных навыков
        ss << "\nДОСТУПНЫЕ НАВЫКИ:\n";
        std::vector<std::string> available;
        available.push_back("skill_solar_flare");
        for (const auto& uid : save.unlocked_upgrades) {
            if (DataManager::getInstance().getUpgrades().count(uid)) {
                const auto& upg = DataManager::getInstance().getUpgrades().at(uid);
                if (upg.type == "active_skill" && !upg.skill_ref.empty()) {
                    if (std::find(available.begin(), available.end(), upg.skill_ref) == available.end()) {
                        available.push_back(upg.skill_ref);
                    }
                }
            }
        }

        for (size_t i = 0; i < available.size(); ++i) {
            if (DataManager::getInstance().getSkills().count(available[i])) {
                const auto& skill = DataManager::getInstance().getSkills().at(available[i]);
                bool isEquipped = std::find(save.equipped_skills.begin(), save.equipped_skills.end(), available[i]) != save.equipped_skills.end();
                as << "[" << (i+1) << "] " << (isEquipped ? "[V] " : "[ ] ") << skill.name << "\n";
            }
        }
        as << "\n[0] Назад";
    }

    m_mainDisplay->setText(sf::String::fromUtf8(ss.str().begin(), ss.str().end()), font, 22, sf::Color::White);
    m_actionMenu->setText(sf::String::fromUtf8(as.str().begin(), as.str().end()), font, 20, sf::Color::Green);
}

/**
 * @brief Обработка ввода для навигации по меню Хаба.
 */
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
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num8)) { anyNum = true; num = 8; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num9)) { anyNum = true; num = 9; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num0)) { anyNum = true; num = 0; }

    if (!anyNum) { m_keyHeld = false; return; }
    if (m_keyHeld) return;
    m_keyHeld = true;

    if (m_currentMenu == HubMenu::Main) {
        if (num == 1) {
            RunManager::getInstance().startNewRun();
            m_stateMachine.changeState(std::make_unique<ExplorationState>(m_window, m_stateMachine));
        } else if (num == 2) m_currentMenu = HubMenu::Shop;
        else if (num == 3) m_currentMenu = HubMenu::Inventory;
    } else if (m_currentMenu == HubMenu::Shop) {
        if (num == 0) m_currentMenu = HubMenu::Main;
        else if (num == 8 && m_shopPage > 0) m_shopPage--;
        else if (num == 9 && (m_shopPage + 1) * 5 < m_shopUpgradeIds.size()) m_shopPage++;
        else if (num > 0 && num <= 5) {
            size_t idx = m_shopPage * 5 + (num - 1);
            if (idx < m_shopUpgradeIds.size()) buyUpgrade((int)idx);
        }
    } else if (m_currentMenu == HubMenu::Inventory) {
        if (num == 0) m_currentMenu = HubMenu::Main;
        else {
             // Логика формирования того же списка available, что и в updateUI
             std::vector<std::string> available;
             available.push_back("skill_solar_flare");
             for (const auto& uid : SaveManager::getInstance().getData().unlocked_upgrades) {
                if (DataManager::getInstance().getUpgrades().count(uid)) {
                    const auto& upg = DataManager::getInstance().getUpgrades().at(uid);
                    if (upg.type == "active_skill" && !upg.skill_ref.empty()) {
                        if (std::find(available.begin(), available.end(), upg.skill_ref) == available.end()) available.push_back(upg.skill_ref);
                    }
                }
             }
             if (num > 0 && (size_t)num <= available.size()) toggleSkill(available[num-1]);
        }
    }

    updateUI();
}

/**
 * @brief Покупка улучшения.
 */
void HubState::buyUpgrade(int index) {
    SaveData& save = SaveManager::getInstance().getData();
    const auto& upg = DataManager::getInstance().getUpgrades().at(m_shopUpgradeIds[index]);
    
    if (save.solis_sparks >= upg.cost) {
        save.solis_sparks -= upg.cost;
        save.unlocked_upgrades.push_back(upg.id);
        SaveManager::getInstance().save();
    }
}

/**
 * @brief Экипировка/снятие навыка. Максимум 3 навыка.
 */
void HubState::toggleSkill(const std::string& skillId) {
    SaveData& save = SaveManager::getInstance().getData();
    auto it = std::find(save.equipped_skills.begin(), save.equipped_skills.end(), skillId);
    if (it != save.equipped_skills.end()) save.equipped_skills.erase(it);
    else if (save.equipped_skills.size() < 3) save.equipped_skills.push_back(skillId);
    SaveManager::getInstance().save();
}

void HubState::update(float deltaTime) {}

void HubState::render(sf::RenderWindow& window) {
    m_topBar->render(window);
    m_mainDisplay->render(window);
    m_actionMenu->render(window);
}

} // namespace solis
