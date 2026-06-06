#include "Managers/DataManager.h"
#include <fstream>
#include <iostream>
#include <random>

namespace solis {

DataManager& DataManager::getInstance() {
    static DataManager instance;
    return instance;
}

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

bool DataManager::loadLore(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    try {
        nlohmann::json j;
        file >> j;
        for (const auto& item : j["lore"]) {
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

} // namespace solis
