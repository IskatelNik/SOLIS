#include "Managers/DataManager.h"
#include <fstream>
#include <iostream>
#include <random>

namespace solis {

/**
 * @brief Синглтон для доступа к глобальным данным игры.
 */
DataManager& DataManager::getInstance() {
    static DataManager instance;
    return instance;
}

/**
 * @brief Загружает базу данных комнат из JSON.
 */
bool DataManager::loadRooms(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[DataManager] Failed to open rooms file: " << filepath << std::endl;
        return false;
    }

    try {
        nlohmann::json j;
        file >> j;

        m_rooms.clear();
        for (const auto& item : j["rooms"]) {
            Room room;
            room.id = item.value("id", "");
            room.level = item.value("level", 1);
            room.type = item.value("type", "combat");
            room.preview_text = item.value("preview_text", "");
            room.description = item.value("description", "");
            
            if (item.contains("possible_enemies")) {
                room.possible_enemies = item["possible_enemies"].get<std::vector<std::string>>();
            }

            m_rooms.push_back(room);
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[DataManager] JSON Error in rooms: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Загружает шаблоны обычных врагов.
 */
bool DataManager::loadEnemies(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    try {
        nlohmann::json j;
        file >> j;
        for (const auto& item : j["enemies"]) {
            m_enemyTemplates[item["id"]] = item;
        }
        return true;
    } catch (...) { return false; }
}

/**
 * @brief Загружает шаблоны боссов и их фаз.
 */
bool DataManager::loadBosses(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    try {
        nlohmann::json j;
        file >> j;
        for (const auto& item : j["bosses"]) {
            m_bossTemplates[item["id"]] = item;
        }
        return true;
    } catch (...) { return false; }
}

/**
 * @brief Загружает активные навыки игрока.
 */
bool DataManager::loadSkills(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    try {
        nlohmann::json j;
        file >> j;
        for (const auto& item : j["skills"]) {
            Skill skill;
            skill.id = item["id"];
            skill.name = item["name"];
            skill.description = item["description"];
            skill.heat_cost_added = item.value("heat_cost_added", 0.0f);
            skill.base_damage = item.value("base_damage", 0);
            
            if (item.contains("effects")) {
                for (const auto& effJ : item["effects"]) {
                    SkillEffect eff;
                    eff.target = effJ["target"];
                    eff.type = effJ["type"];
                    eff.value = effJ["value"];
                    eff.duration_turns = effJ["duration_turns"];
                    skill.effects.push_back(eff);
                }
            }
            m_skills[skill.id] = skill;
        }
        return true;
    } catch (...) { return false; }
}

/**
 * @brief Загружает записи лора для социальной системы боя.
 */
bool DataManager::loadLore(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    try {
        nlohmann::json j;
        file >> j;
        if (!j.contains("lore_items")) return false;
        
        for (const auto& item : j["lore_items"]) {
            LoreItem lore;
            lore.id = item["id"];
            lore.level = item.value("level", 1);
            lore.target_trait = item.value("target_trait", -1);
            lore.pickup_text = item["pickup_text"];
            lore.dialogue_option_text = item["dialogue_option_text"];
            m_lore[lore.id] = lore;
        }
        return true;
    } catch (...) { return false; }
}

/**
 * @brief Загружает доступные улучшения в Хабе.
 */
bool DataManager::loadUpgrades(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    try {
        nlohmann::json j;
        file >> j;
        for (const auto& item : j["upgrades"]) {
            Upgrade upg;
            upg.id = item["id"];
            upg.name = item["name"];
            upg.description = item["description"];
            upg.cost = item.value("cost", 50);
            upg.type = item.value("type", "passive_stat");
            upg.skill_ref = item.value("skill_ref", "");
            upg.effect_target = item.value("effect_target", "");
            upg.effect_value = item.value("effect_value", 0.0f);
            m_upgrades[upg.id] = upg;
        }
        return true;
    } catch (...) { return false; }
}

/**
 * @brief Загружает базу данных артефактов.
 */
bool DataManager::loadArtifacts(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    try {
        nlohmann::json j;
        file >> j;
        for (const auto& item : j["artifacts"]) {
            Artifact art;
            art.id = item["id"];
            art.name = item["name"];
            art.description = item["description"];
            art.modifier_type = item["modifier_type"];
            art.value = item["value"];
            m_artifacts[art.id] = art;
        }
        return true;
    } catch (...) { return false; }
}

/**
 * @brief Загружает текстовые квесты/события, сгруппированные по уровням.
 */
bool DataManager::loadEvents(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    try {
        nlohmann::json j;
        file >> j;
        if (!j.contains("events")) return false;

        const auto& levels = j["events"];
        for (auto it = levels.begin(); it != levels.end(); ++it) {
            std::string levelStr = it.key(); 
            int level = 1;
            try {
                if (levelStr.find("level_") == 0) {
                    level = std::stoi(levelStr.substr(6));
                }
            } catch (...) {}

            int order_index = 0;
            for (const auto& item : it.value()) {
                Event ev;
                ev.id = item["id"];
                ev.level = level;
                ev.order_index = order_index++;
                ev.preview_text = item.value("preview_text", "");
                ev.description = item["description"];
                
                if (item.contains("choices")) {
                    for (const auto& choiceJ : item["choices"]) {
                        EventChoice choice;
                        choice.text = choiceJ.value("text", "");
                        choice.heat_change = choiceJ.value("heat_change", 0.0f);
                        choice.ideology_change = choiceJ.value("ideology_change", 0);
                        ev.choices.push_back(choice);
                    }
                }

                m_events[ev.id] = ev;
            }
        }
        return true;
    } catch (...) { return false; }
}

/**
 * @brief Создает новый экземпляр врага на основе ID шаблона.
 */
std::unique_ptr<Enemy> DataManager::spawnEnemy(const std::string& id) {
    if (m_enemyTemplates.find(id) == m_enemyTemplates.end()) return nullptr;

    const auto& j = m_enemyTemplates[id];
    auto enemy = std::make_unique<Enemy>();
    enemy->setId(id);
    enemy->setName(j.value("name", "Unknown"));
    enemy->setMaxHp(j.value("max_hp", 50));
    enemy->setBaseDamage(j.value("base_damage", 10));
    enemy->setSparksReward(j.value("sparks_reward", 10));
    enemy->setEmpathyRevealThreshold(j.value("empathy_reveal_threshold", 20));

    // Выбор случайной черты характера для социальной боевки
    if (j.contains("possible_traits")) {
        const auto& traits = j["possible_traits"];
        if (!traits.empty()) {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, static_cast<int>(traits.size()) - 1);
            
            const auto& traitJ = traits[dis(gen)];
            enemy->setTrait(
                static_cast<Trait>(traitJ.value("trait", 0)),
                traitJ.value("trait_name_hidden", "???"),
                traitJ.value("intro_text", "...")
            );
        }
    }

    return enemy;
}

/**
 * @brief Создает новый экземпляр босса на основе ID шаблона.
 */
std::unique_ptr<BossEnemy> DataManager::spawnBoss(const std::string& id) {
    if (m_bossTemplates.find(id) == m_bossTemplates.end()) return nullptr;

    const auto& j = m_bossTemplates[id];
    auto boss = std::make_unique<BossEnemy>();
    boss->setId(id);
    boss->setName(j.value("name", "Unknown Boss"));
    boss->setMaxHp(j.value("max_hp", 200));
    boss->setBaseDamage(j.value("base_damage", 15));
    boss->setMaxWillpower(j.value("max_willpower", 3));
    boss->setSparksReward(j.value("sparks_reward", 50));

    if (j.contains("phases")) {
        for (const auto& pJ : j["phases"]) {
            BossPhase p;
            p.willpower_left = pJ.value("willpower_left", 0);
            p.target_trait = static_cast<Trait>(pJ.value("target_trait", 0));
            p.target_trait_name = pJ.value("target_trait_name", "???");
            p.boss_replica = pJ.value("boss_replica", "...");
            p.success_reply = pJ.value("success_reply", "Успех!");
            p.fail_reply = pJ.value("fail_reply", "Провал!");
            p.aggression_turns = pJ.value("aggression_turns", 0);
            boss->addPhase(p);
        }
    }
    
    boss->updatePhase();

    return boss;
}

} // namespace solis
