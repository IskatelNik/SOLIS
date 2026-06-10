#include "States/CombatState.h"
#include "States/HubState.h"
#include "States/EndingState.h"
#include "Core/ResourceManager.h"
#include "Managers/RunManager.h"
#include "Utils/GameConstans.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <random>

namespace solis {

CombatState::CombatState(sf::RenderWindow& window, StateMachine& stateMachine, std::unique_ptr<Enemy> enemy)
    : m_window(window), m_stateMachine(stateMachine), m_enemy(std::move(enemy)), 
      m_player(RunManager::getInstance().getPlayer()) {}

void CombatState::init() {
    sf::Vector2u windowSize = m_window.getSize();
    float w = static_cast<float>(windowSize.x);
    float h = static_cast<float>(windowSize.y);

    const sf::Font& font = ResourceManager::getInstance().getFont("main");

    // Layout from Constants
    m_topBar = std::make_unique<UIBox>(sf::Vector2f(0,0), sf::Vector2f(w, h * constants::UI_TOPBAR_HEIGHT), constants::COLOR_UI_BG_LIGHT, constants::COLOR_UI_OUTLINE, -2.f);
    m_logDisplay = std::make_unique<UIBox>(sf::Vector2f(0, h * constants::UI_TOPBAR_HEIGHT), sf::Vector2f(w * 0.7f, h * constants::UI_MAIN_DISPLAY_HEIGHT), sf::Color::Black, constants::COLOR_UI_OUTLINE, -2.f);
    m_enemyDisplay = std::make_unique<UIBox>(sf::Vector2f(w * 0.7f, h * constants::UI_TOPBAR_HEIGHT), sf::Vector2f(w * 0.3f, h * constants::UI_MAIN_DISPLAY_HEIGHT), constants::COLOR_UI_BG_DARK, constants::COLOR_UI_OUTLINE, -2.f);
    
    m_actionMenu = std::make_unique<UIBox>(sf::Vector2f(0, h * (constants::UI_TOPBAR_HEIGHT + constants::UI_MAIN_DISPLAY_HEIGHT)), sf::Vector2f(w * constants::UI_ACTION_SPLIT_LEFT, h * constants::UI_ACTION_MENU_HEIGHT), constants::COLOR_UI_BG_DARK, constants::COLOR_UI_OUTLINE, -2.f);
    m_playerStatus = std::make_unique<UIBox>(sf::Vector2f(w * constants::UI_ACTION_SPLIT_LEFT, h * (constants::UI_TOPBAR_HEIGHT + constants::UI_MAIN_DISPLAY_HEIGHT)), sf::Vector2f(w * constants::UI_ACTION_SPLIT_RIGHT, h * constants::UI_ACTION_MENU_HEIGHT), sf::Color(15,15,15), constants::COLOR_UI_OUTLINE, -2.f);

    logMessage(m_enemy->getIntroText());
    m_keyHeld = true; 

    // Level 2 Biome Modifier: Enemy gets +0.2 defense
    if (RunManager::getInstance().getCurrentLevel() == 2) {
        m_enemy->setDefenseModifier(m_enemy->getDefenseModifier() + 0.2f);
        logMessage("ВЛИЯНИЕ УРОВНЯ: Враг укреплен (+0.2 DEF)");
    }

    if (m_player.getHeat() >= constants::HEAT_MAX) {
        logMessage("КРИТИЧЕСКИЙ ПЕРЕГРЕВ! Вы сгораете заживо...");
        m_player.takeDamage(999);
        m_isCombatOver = true;
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
    
    sf::Color heatColor = (m_player.getHeat() > constants::HEAT_OVERLOAD_THRESHOLD) ? constants::COLOR_HEAT_OVERLOAD : constants::COLOR_HEAT_NORMAL;
    m_playerStatus->setProgressBar(m_player.getHeat() / constants::HEAT_MAX, heatColor);

    std::stringstream ess;
    ess << m_enemy->getName() << "\n\n";
    ess << "HP: " << m_enemy->getCurrentHp() << "/" << m_enemy->getMaxHp() << "\n";
    ess << "DEF MOD: " << std::fixed << std::setprecision(2) << m_enemy->getDefenseModifier() << "\n";
    
    if (m_enemy->isBoss()) {
        auto boss = static_cast<BossEnemy*>(m_enemy.get());
        ess << "WILLPOWER: " << boss->getCurrentWillpower() << "/" << boss->getMaxHp() << "\n"; // Just willpower info
    }

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
        menuText = "[1] Атака\n[2] Манипуляция\n";
        if (m_enemy->isBoss() && static_cast<BossEnemy*>(m_enemy.get())->getAggressionTurns() > 0) {
            menuText += "[3] Договориться (Заблокировано)";
        } else {
            menuText += "[3] Договориться";
        }
    } else if (m_currentMenu == CombatMenu::Attack) {
        const auto& skills = m_player.getActiveSkills();
        for (size_t i = 0; i < skills.size(); ++i) {
            menuText += "[" + std::to_string(i+1) + "] " + skills[i].name + "\n";
        }
        menuText += "[0] Назад";
    } else if (m_currentMenu == CombatMenu::ConfirmSkill) {
        const auto& skills = m_player.getActiveSkills();
        const auto& skill = skills[m_selectedSkillIndex];
        
        float heatMultiplier = 1.0f + (m_player.getHeat() / constants::HEAT_MAX);
        int estDmg = (int)(skill.base_damage * heatMultiplier * m_enemy->getDefenseModifier());
        float finalHeatCost = m_player.calculateHeatGain(skill.heat_cost_added);

        // MVP 5 Biome 3 Update: -40% damage, -70% heat cost
        if (RunManager::getInstance().getCurrentLevel() == 3) {
            estDmg = (int)(estDmg * 0.6f);
            finalHeatCost *= 0.3f;
        }

        menuText = skill.name + "\n" + skill.description + "\n";
        menuText += "УРОН (актуальный): " + std::to_string(estDmg) + " | + ЖАР: " + std::to_string((int)finalHeatCost) + "%\n";
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

    // Уровень 4 Biome Modifier: +10 Heat at start of player turn
    static bool level4HeatApplied = false;
    if (RunManager::getInstance().getCurrentLevel() == 4 && !level4HeatApplied) {
        m_player.addHeat(10.0f);
        logMessage("ВЛИЯНИЕ УРОВНЯ: Вы получаете 10% Жара из атмосферы!");
        level4HeatApplied = true;
    }

    if (m_currentMenu == CombatMenu::Main) {
        if (num == 1) m_currentMenu = CombatMenu::Attack;
        else if (num == 2) m_currentMenu = CombatMenu::Manipulation;
        else if (num == 3) {
            if (m_enemy->isBoss() && static_cast<BossEnemy*>(m_enemy.get())->getAggressionTurns() > 0) {
                // Blocked
            } else {
                m_dialogueOptions = DialogueGenerator::generateOptions(m_enemy->getTrait(), RunManager::getInstance().getCurrentLevel());
                m_currentMenu = CombatMenu::Negotiate;
            }
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
            float heatMultiplier = 1.0f + (m_player.getHeat() / constants::HEAT_MAX);
            int dmg = (int)(skill.base_damage * heatMultiplier * m_enemy->getDefenseModifier());
            
            // MVP 5: Артефакты влияют на получение Жара
            float finalHeatCost = m_player.calculateHeatGain(skill.heat_cost_added);
            
            // MVP 5 Biome 3 Update: -40% damage, -70% heat cost
            if (RunManager::getInstance().getCurrentLevel() == 3) {
                dmg = (int)(dmg * 0.6f);
                finalHeatCost *= 0.3f;
                logMessage("ВЛИЯНИЕ УРОВНЯ: Энергия рассеивается во тьме (-Урон, -Жар).");
            }

            m_enemy->takeDamage(dmg);
            m_player.addHeat(finalHeatCost);
            
            // Накладываем эффекты
            for (const auto& eff : skill.effects) {
                if (eff.target == "enemy") m_enemy->addStatusEffect({eff.type, eff.value, eff.duration_turns});
                else m_player.addStatusEffect({eff.type, eff.value, eff.duration_turns});
            }

            logMessage("Вы используете " + skill.name + ". Урон: " + std::to_string(dmg));
            
            m_playerTurn = false;
            level4HeatApplied = false;
            m_currentMenu = CombatMenu::Main;
        } else if (num == 0) {
            m_currentMenu = CombatMenu::Attack;
        }
    } else if (m_currentMenu == CombatMenu::Negotiate) {
        if (num == 0) m_currentMenu = CombatMenu::Main;
        else if (num > 0 && (size_t)num <= m_dialogueOptions.size()) {
            const auto& opt = m_dialogueOptions[num - 1];
            
            if (m_enemy->isBoss()) {
                auto boss = static_cast<BossEnemy*>(m_enemy.get());
                const auto& phase = boss->getCurrentPhase();

                if (opt.isCorrect) {
                    logMessage(phase.success_reply);
                    RunManager::getInstance().addEmpathy(constants::EMPATHY_REWARD_CORRECT);
                    
                    boss->reduceWillpower();
                    m_player.reduceHeat(20.0f); // TDD: Жар снижается
                    
                    if (boss->getCurrentWillpower() <= 0) {
                        m_isSocialVictory = true;
                        m_isCombatOver = true;
                    } else {
                        // Boss transitions to next phase, print new replica
                        logMessage("--- ФАЗА ИЗМЕНЕНА ---");
                        logMessage(boss->getCurrentPhase().boss_replica); 
                        
                        // Используем длительность агрессии из НОВОЙ (текущей) фазы
                        boss->setAggressionTurns(boss->getCurrentPhase().aggression_turns);
                        m_justNegotiated = true; // Запрещаем списывать 1 ход агрессии прямо сейчас
                        
                        m_playerTurn = false;
                        level4HeatApplied = false;
                        m_currentMenu = CombatMenu::Main;
                    }
                } else {
                    logMessage(phase.fail_reply);
                    m_enemy->setDamageModifier(m_enemy->getDamageModifier() + 0.3f);
                    RunManager::getInstance().addEmpathy(constants::EMPATHY_REWARD_WRONG);
                    
                    m_playerTurn = false;
                    level4HeatApplied = false;
                    m_currentMenu = CombatMenu::Main;
                }
            } else {
                // Обычный враг
                if (opt.isCorrect) {
                    logMessage("Вы подобрали верные слова! Враг опускает оружие.");
                    RunManager::getInstance().addEmpathy(constants::EMPATHY_REWARD_CORRECT);
                    m_isSocialVictory = true;
                    m_isCombatOver = true;
                } else if (opt.isNeutral) {
                    logMessage("Ваши слова не трогают врага. Он игнорирует вас.");
                    RunManager::getInstance().addEmpathy(constants::EMPATHY_REWARD_NEUTRAL);
                    m_playerTurn = false;
                    level4HeatApplied = false;
                    m_currentMenu = CombatMenu::Main;
                } else {
                    logMessage("Ваши слова ввергают врага в ярость!");
                    m_enemy->setDamageModifier(m_enemy->getDamageModifier() + 0.3f);
                    RunManager::getInstance().addEmpathy(constants::EMPATHY_REWARD_WRONG);
                    
                    m_playerTurn = false;
                    level4HeatApplied = false;
                    m_currentMenu = CombatMenu::Main;
                }
            }
        }
    } else if (m_currentMenu == CombatMenu::Manipulation) {
        if (num == 0) m_currentMenu = CombatMenu::Main;
        else if (num == 1) { // Take
            if (m_enemy->isDrainable()) {
                m_player.addHeat(constants::MANIP_TAKE_HEAT_GAIN);
                m_enemy->setDefenseModifier(m_enemy->getDefenseModifier() + constants::MANIP_TAKE_DEF_DEBUFF);
                m_enemy->setDrainable(false);
                logMessage("Вы поглощаете энергию!");
                m_playerTurn = false;
                m_currentMenu = CombatMenu::Main;
            } else {
                logMessage("Враг пуст! Нужно сначала отдать жар.");
            }
        } else if (num == 2) { // Give
            if (m_player.getHeat() > 0) {
                float heatToVent = m_player.getHeat() * constants::MANIP_GIVE_HEAT_VENT_PERCENT;
                m_player.reduceHeat(heatToVent);
                m_player.heal((int)(heatToVent * constants::MANIP_GIVE_HEAL_MULT));
                m_enemy->setDamageModifier(m_enemy->getDamageModifier() + constants::MANIP_GIVE_DMG_BUFF);
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
        processTurnEnd();
    }
    
    checkEndCombat();
    updateUI();
}

void CombatState::processTurnEnd() {
    m_player.processTurnEffects();
    m_enemy->processTurnEffects();
    
    if (m_enemy->isBoss()) {
        static_cast<BossEnemy*>(m_enemy.get())->decrementAggression();
    }
    
    m_playerTurn = true;
}

void CombatState::enemyTurn() {
    int dmg = (int)(m_enemy->getBaseDamage() * m_enemy->getDamageModifier());
    m_player.takeDamage(dmg);
    logMessage(m_enemy->getName() + " атакует! Урон: " + std::to_string(dmg));

    if (m_player.getHeat() > constants::HEAT_OVERLOAD_THRESHOLD) {
        int overloadDmg = (int)((m_player.getHeat() - constants::HEAT_OVERLOAD_THRESHOLD) * constants::HEAT_DAMAGE_MULT);
        m_player.takeDamage(overloadDmg);
        logMessage("ПЕРЕГРУЗКА! Системы горят. Урон: " + std::to_string(overloadDmg));
    }
}

void CombatState::checkEndCombat() {
    if (m_enemy->getCurrentHp() <= 0) {
        logMessage("ПОБЕДА! Враг повержен. +10 Искр.");
        RunManager::getInstance().addSparks(10);
        RunManager::getInstance().incrementBloodCounter();
        if (m_enemy->isBoss()) {
            RunManager::getInstance().modifyIdeologyScore(-1);
        }
        m_isCombatOver = true;
    } else if (m_isSocialVictory) {
        RunManager::getInstance().incrementMercyCounter();
        if (m_enemy->isBoss()) {
            RunManager::getInstance().modifyIdeologyScore(1);
        }
        
        int currentLvl = RunManager::getInstance().getCurrentLevel();
        if (currentLvl == 3) {
            // MVP 5 Biome 3 Update: 35% chance to lose artifact instead of gaining one
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1, 100);
            int roll = dis(gen);
            
            if (roll <= 35) {
                std::string lostArt = m_player.removeRandomArtifact();
                if (!lostArt.empty()) {
                    logMessage("ВЛИЯНИЕ УРОВНЯ: Тьма поглотила ваш артефакт (" + lostArt + ")!");
                } else {
                    logMessage("ВЛИЯНИЕ УРОВНЯ: Тьма тянется к вам, но забирать нечего.");
                }
            } else {
                logMessage("ВЛИЯНИЕ УРОВНЯ: Тьма безмолвствует. Вы ничего не получили.");
            }
        } else {
            // Выдача артефакта за мирное разрешение (Стандарт для уровней 1, 2, 4)
            const auto& artifacts = DataManager::getInstance().getArtifacts();
            if (!artifacts.empty()) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, static_cast<int>(artifacts.size()) - 1);
                
                auto it = artifacts.begin();
                std::advance(it, dis(gen));
                m_player.addArtifact(it->second);
                logMessage("Милость вознаграждена! Получен артефакт: " + it->second.name);
            }
        }

        m_isCombatOver = true;
    } else if (m_player.getCurrentHp() <= 0) {
        logMessage("ПОРАЖЕНИЕ... Ваша искра угасла.");
        m_isCombatOver = true;
    }
}

void CombatState::endCombat(bool victory) {
    if (victory) {
        if (m_isSocialVictory) {
            RunManager::getInstance().addSparks(5);
        }
        
        if (m_enemy->isBoss()) {
            if (RunManager::getInstance().getCurrentLevel() >= 4) {
                m_stateMachine.clearAndSetState(std::make_unique<EndingState>(m_window, m_stateMachine));
            } else {
                RunManager::getInstance().advanceLevel();
                m_stateMachine.popState();
            }
        } else {
            m_stateMachine.popState();
        }
    } else {
        m_stateMachine.clearAndSetState(std::make_unique<HubState>(m_window, m_stateMachine));
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
