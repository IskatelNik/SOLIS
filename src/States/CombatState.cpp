#include "States/CombatState.h"
#include "Core/ResourceManager.h"
#include "Managers/RunManager.h"
#include <sstream>
#include <iomanip>

namespace solis {

CombatState::CombatState(sf::RenderWindow& window, StateMachine& stateMachine, std::unique_ptr<Enemy> enemy)
    : m_window(window), m_stateMachine(stateMachine), m_enemy(std::move(enemy)), 
      m_player(RunManager::getInstance().getPlayer()) {}

void CombatState::init() {
    sf::Vector2u windowSize = m_window.getSize();
    float w = static_cast<float>(windowSize.x);
    float h = static_cast<float>(windowSize.y);

    const sf::Font& font = ResourceManager::getInstance().getFont("main");

    m_topBar = std::make_unique<UIBox>(sf::Vector2f(0,0), sf::Vector2f(w, h*0.1f), sf::Color(30,30,30), sf::Color::White, -2.f);
    m_logDisplay = std::make_unique<UIBox>(sf::Vector2f(0, h*0.1f), sf::Vector2f(w*0.7f, h*0.55f), sf::Color(10,10,10), sf::Color::White, -2.f);
    m_enemyDisplay = std::make_unique<UIBox>(sf::Vector2f(w*0.7f, h*0.1f), sf::Vector2f(w*0.3f, h*0.55f), sf::Color(20,20,20), sf::Color::White, -2.f);
    
    m_actionMenu = std::make_unique<UIBox>(sf::Vector2f(0, h*0.65f), sf::Vector2f(w*0.7f, h*0.35f), sf::Color(20,20,20), sf::Color::White, -2.f);
    m_playerStatus = std::make_unique<UIBox>(sf::Vector2f(w*0.7f, h*0.65f), sf::Vector2f(w*0.3f, h*0.35f), sf::Color(15,15,15), sf::Color::White, -2.f);

    logMessage(m_enemy->getIntroText());
    
    if (m_player.getHeat() >= 100.0f) {
        logMessage("КРИТИЧЕСКИЙ ПЕРЕГРЕВ! Вы сгораете заживо...");
        m_player.takeDamage(999);
        checkEndCombat();
    }

    updateUI();
}

void CombatState::logMessage(const std::string& msg) {
    m_logs.push_back(msg);
    if (m_logs.size() > 15) m_logs.pop_front();
    
    std::string fullLog;
    for (const auto& l : m_logs) fullLog += l + "\n";
    
    const sf::Font& font = ResourceManager::getInstance().getFont("main");
    m_logDisplay->setText(sf::String::fromUtf8(fullLog.begin(), fullLog.end()), font, 18, sf::Color::White);
}

void CombatState::updateUI() {
    const sf::Font& font = ResourceManager::getInstance().getFont("main");
    
    std::string locInfo = "В БОЮ | Сектор: ЗАГЛУШКА | Уровень: " + std::to_string(RunManager::getInstance().getCurrentLevel());
    m_topBar->setText(sf::String::fromUtf8(locInfo.begin(), locInfo.end()), font, 24, sf::Color::White);

    std::stringstream pss;
    pss << "HP: " << m_player.getCurrentHp() << "/" << m_player.getMaxHp() << "\n";
    pss << "HEAT: " << (int)m_player.getHeat() << "%";
    pss << "\nEMP: " << m_player.getEmpathy();
    std::string pStr = pss.str();
    m_playerStatus->setText(sf::String::fromUtf8(pStr.begin(), pStr.end()), font, 22, sf::Color::White);
    
    sf::Color heatColor = (m_player.getHeat() > 85.0f) ? sf::Color::Red : sf::Color::Yellow;
    m_playerStatus->setProgressBar(m_player.getHeat() / 100.0f, heatColor);

    std::stringstream ess;
    ess << m_enemy->getName() << "\n\n";
    ess << "HP: " << m_enemy->getCurrentHp() << "/" << m_enemy->getMaxHp() << "\n";
    ess << "DEF MOD: " << std::fixed << std::setprecision(2) << m_enemy->getDefenseModifier() << "\n";
    
    std::string traitName = "???";
    if (m_player.getEmpathy() >= m_enemy->getEmpathyRevealThreshold()) {
        traitName = m_enemy->getTraitNameHidden();
    }
    ess << "Trait: " << traitName;

    std::string enemyStr = ess.str();
    m_enemyDisplay->setText(sf::String::fromUtf8(enemyStr.begin(), enemyStr.end()), font, 20, sf::Color::White);

    std::string menuText;
    if (m_isCombatOver) {
        menuText = "[Space] Продолжить...";
    } else if (m_currentMenu == CombatMenu::Main) {
        menuText = "[1] Атака\n[2] Манипуляция\n[3] Договориться";
    } else if (m_currentMenu == CombatMenu::Attack) {
        const auto& skills = m_player.getActiveSkills();
        for (size_t i = 0; i < skills.size(); ++i) {
            menuText += "[" + std::to_string(i+1) + "] " + skills[i].name + "\n";
        }
        menuText += "[0] Назад";
    } else if (m_currentMenu == CombatMenu::ConfirmSkill) {
        const auto& skills = m_player.getActiveSkills();
        const auto& skill = skills[m_selectedSkillIndex];
        
        float heatMultiplier = 1.0f + (m_player.getHeat() / 100.0f);
        int estDmg = (int)(skill.base_damage * heatMultiplier * m_enemy->getDefenseModifier());

        menuText = skill.name + "\n" + skill.description + "\n";
        menuText += "УРОН (актуальный): " + std::to_string(estDmg) + " | + ЖАР: " + std::to_string((int)skill.heat_cost_added) + "%\n";
        menuText += "[1] ПОДТВЕРДИТЬ\n[0] Отмена";
    } else if (m_currentMenu == CombatMenu::Negotiate) {
        for (size_t i = 0; i < m_dialogueOptions.size(); ++i) {
            menuText += "[" + std::to_string(i + 1) + "] " + m_dialogueOptions[i].text + "\n";
        }
        menuText += "[0] Назад";
    } else if (m_currentMenu == CombatMenu::Manipulation) {
        std::string drainStatus = m_enemy->isDrainable() ? "(Доступно)" : "(Пуст)";
        menuText = "[1] Поглотить (+15% HEAT, -0.2 DEF) " + drainStatus + "\n[2] Отдать (-20% HEAT, Heal)\n[0] Назад";
    }
    m_actionMenu->setText(sf::String::fromUtf8(menuText.begin(), menuText.end()), font, 22, sf::Color::Green);
}

void CombatState::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) m_window.close();

    bool anyNum = false;
    int num = -1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) { anyNum = true; num = 1; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2)) { anyNum = true; num = 2; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3)) { anyNum = true; num = 3; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4)) { anyNum = true; num = 4; }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num0)) { anyNum = true; num = 0; }
    
    bool space = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (!anyNum && !space) { m_keyHeld = false; return; }
    if (m_keyHeld) return;
    m_keyHeld = true;

    if (m_isCombatOver) {
        if (space) endCombat(m_enemy->getCurrentHp() <= 0 || m_isSocialVictory);
        return;
    }

    if (!m_playerTurn) return;

    if (m_currentMenu == CombatMenu::Main) {
        if (num == 1) m_currentMenu = CombatMenu::Attack;
        else if (num == 2) m_currentMenu = CombatMenu::Manipulation;
        else if (num == 3) {
            m_dialogueOptions = DialogueGenerator::generateOptions(m_enemy->getTrait(), RunManager::getInstance().getCurrentLevel());
            m_currentMenu = CombatMenu::Negotiate;
        }
    } else if (m_currentMenu == CombatMenu::Attack) {
        if (num == 0) m_currentMenu = CombatMenu::Main;
        else {
            const auto& skills = m_player.getActiveSkills();
            if (num > 0 && (size_t)num <= skills.size()) {
                m_selectedSkillIndex = num - 1;
                m_currentMenu = CombatMenu::ConfirmSkill;
            }
        }
    } else if (m_currentMenu == CombatMenu::ConfirmSkill) {
        if (num == 1) {
            const auto& skill = m_player.getActiveSkills()[m_selectedSkillIndex];
            float heatMultiplier = 1.0f + (m_player.getHeat() / 100.0f);
            int dmg = (int)(skill.base_damage * heatMultiplier * m_enemy->getDefenseModifier());
            
            m_enemy->takeDamage(dmg);
            m_player.addHeat(skill.heat_cost_added);
            logMessage("Вы используете " + skill.name + ". Урон: " + std::to_string(dmg));
            
            m_playerTurn = false;
            m_currentMenu = CombatMenu::Main;
        } else if (num == 0) {
            m_currentMenu = CombatMenu::Attack;
        }
    } else if (m_currentMenu == CombatMenu::Negotiate) {
        if (num == 0) m_currentMenu = CombatMenu::Main;
        else if (num > 0 && (size_t)num <= m_dialogueOptions.size()) {
            const auto& opt = m_dialogueOptions[num - 1];
            if (opt.isCorrect) {
                logMessage("Вы подобрали верные слова! Враг опускает оружие.");
                m_player.addEmpathy(10);
                m_isSocialVictory = true;
                m_isCombatOver = true;
            } else if (opt.isNeutral) {
                logMessage("Ваши слова не трогают врага. Он игнорирует вас.");
                m_player.addEmpathy(5);
                m_playerTurn = false;
                m_currentMenu = CombatMenu::Main;
            } else {
                logMessage("Ваши слова ввергают врага в ярость!");
                m_enemy->setDamageModifier(m_enemy->getDamageModifier() + 0.3f);
                m_player.addEmpathy(2);
                m_playerTurn = false;
                m_currentMenu = CombatMenu::Main;
            }
        }
    } else if (m_currentMenu == CombatMenu::Manipulation) {
        if (num == 0) m_currentMenu = CombatMenu::Main;
        else if (num == 1) { // Take
            if (m_enemy->isDrainable()) {
                m_player.addHeat(15.0f);
                m_enemy->setDefenseModifier(m_enemy->getDefenseModifier() - 0.2f);
                m_enemy->setDrainable(false);
                logMessage("Вы поглощаете энергию!");
                m_playerTurn = false;
                m_currentMenu = CombatMenu::Main;
            } else {
                logMessage("Враг пуст! Нужно сначала отдать жар.");
            }
        } else if (num == 2) { // Give
            if (m_player.getHeat() > 0) {
                float heatGiven = m_player.getHeat() * 0.2f;
                m_player.reduceHeat(heatGiven);
                m_player.heal((int)(heatGiven * 0.5f));
                m_enemy->setDamageModifier(m_enemy->getDamageModifier() + 0.2f);
                m_enemy->setDrainable(true);
                logMessage("Вы отдаете жар! Враг заряжен.");
                m_playerTurn = false;
                m_currentMenu = CombatMenu::Main;
            } else {
                logMessage("Нет жара для отдачи!");
            }
        }
    }

    if (!m_playerTurn && !m_isCombatOver) {
        enemyTurn();
    }
    
    checkEndCombat();
    updateUI();
}

void CombatState::enemyTurn() {
    int dmg = (int)(m_enemy->getBaseDamage() * m_enemy->getDamageModifier());
    m_player.takeDamage(dmg);
    logMessage(m_enemy->getName() + " атакует! Урон: " + std::to_string(dmg));

    if (m_player.getHeat() > 85.0f) {
        int overloadDmg = (int)((m_player.getHeat() - 85.0f) * 0.5f);
        m_player.takeDamage(overloadDmg);
        logMessage("ПЕРЕГРУЗКА! Системы горят. Урон: " + std::to_string(overloadDmg));
    }
    
    m_playerTurn = true;
}

void CombatState::checkEndCombat() {
    if (m_enemy->getCurrentHp() <= 0) {
        logMessage("ПОБЕДА! Враг повержен.");
        m_isCombatOver = true;
    } else if (m_player.getCurrentHp() <= 0) {
        logMessage("ПОРАЖЕНИЕ... Ваша искра угасла.");
        m_isCombatOver = true;
    }
}

void CombatState::endCombat(bool victory) {
    if (victory) {
        m_stateMachine.popState();
    } else {
        RunManager::getInstance().startNewRun();
        m_stateMachine.popState();
    }
}

void CombatState::update(float deltaTime) {}

void CombatState::render(sf::RenderWindow& window) {
    m_topBar->render(window);
    m_logDisplay->render(window);
    m_enemyDisplay->render(window);
    m_actionMenu->render(window);
    m_playerStatus->render(window);
}

} // namespace solis
