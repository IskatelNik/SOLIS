#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "Entities/Enemy.h"
#include "Mechanics/Skill.h"
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

class DataManager {
public:
    static DataManager& getInstance();

    bool loadRooms(const std::string& filepath);
    bool loadEnemies(const std::string& filepath);
    bool loadSkills(const std::string& filepath);

    const std::vector<Room>& getRooms() const { return m_rooms; }
    
    // MVP 2: Получение врага по ID и список навыков
    std::unique_ptr<Enemy> spawnEnemy(const std::string& id);
    const std::map<std::string, Skill>& getSkills() const { return m_skills; }

private:
    DataManager() = default;
    ~DataManager() = default;
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    std::vector<Room> m_rooms;
    std::map<std::string, nlohmann::json> m_enemyTemplates;
    std::map<std::string, Skill> m_skills;
};

} // namespace solis

#endif // DATA_MANAGER_H
