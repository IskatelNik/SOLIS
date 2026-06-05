#include "Managers/DataManager.h"
#include <fstream>
#include <iostream>

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
        std::cerr << "[DataManager] JSON Error: " << e.what() << std::endl;
        return false;
    }
}

} // namespace solis
