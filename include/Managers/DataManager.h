#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "Entities/Enemy.h"
#include "Entities/BossEnemy.h"
#include "Mechanics/Skill.h"
#include "Mechanics/Artifact.h"
#include "Mechanics/Event.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>

namespace solis {

struct Room {
    std::string id;
    int level;
    std::string type;
    std::string preview_text;
    std::string description;
    std::vector<std::string> possible_enemies;
};

struct LoreItem {
    std::string id;
    int level;
    int target_trait; // -1 for placeholders
    std::string pickup_text;
    std::string dialogue_option_text;
};

struct Upgrade {
    std::string id;
    std::string name;
    std::string description;
    int cost;
    std::string type;      // "passive_stat" or "active_skill"
    std::string skill_ref; // For active skills
    std::string effect_target; // e.g., "max_hp", "overload_threshold"
    float effect_value;
};

class DataManager {
public:
    static DataManager& getInstance();

    bool loadRooms(const std::string& filepath);
    bool loadEnemies(const std::string& filepath);
    bool loadBosses(const std::string& filepath);
    bool loadSkills(const std::string& filepath);
    bool loadLore(const std::string& filepath);
    bool loadUpgrades(const std::string& filepath);
    bool loadArtifacts(const std::string& filepath);
    bool loadEvents(const std::string& filepath);

    const std::vector<Room>& getRooms() const { return m_rooms; }
    
    std::unique_ptr<Enemy> spawnEnemy(const std::string& id);
    std::unique_ptr<BossEnemy> spawnBoss(const std::string& id);

    const std::map<std::string, Skill>& getSkills() const { return m_skills; }
    const std::map<std::string, LoreItem>& getLore() const { return m_lore; }
    const std::map<std::string, Upgrade>& getUpgrades() const { return m_upgrades; }
    const std::map<std::string, Artifact>& getArtifacts() const { return m_artifacts; }
    const std::map<std::string, Event>& getEvents() const { return m_events; }

private:
    DataManager() = default;
    ~DataManager() = default;
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    std::vector<Room> m_rooms;
    std::map<std::string, nlohmann::json> m_enemyTemplates;
    std::map<std::string, nlohmann::json> m_bossTemplates;
    std::map<std::string, Skill> m_skills;
    std::map<std::string, LoreItem> m_lore;
    std::map<std::string, Upgrade> m_upgrades;
    std::map<std::string, Artifact> m_artifacts;
    std::map<std::string, Event> m_events;
};

} // namespace solis

#endif // DATA_MANAGER_H
