#include "Managers/SaveManager.h"
#include "Utils/GameConstans.h"
#include <fstream>
#include <iostream>

namespace solis {

SaveManager& SaveManager::getInstance() {
    static SaveManager instance;
    return instance;
}

bool SaveManager::load() {
    std::ifstream file(constants::SAVE_FILE_PATH);
    if (!file.is_open()) {
        std::cout << "[SaveManager] No save file found, creating new one." << std::endl;
        m_data = SaveData();
        return save();
    }

    try {
        nlohmann::json j;
        file >> j;

        m_data.solis_sparks = j.value("solis_sparks", 0);
        m_data.empathy_level = j.value("empathy_level", 0);
        m_data.unlocked_lore = j.value("unlocked_lore", std::vector<std::string>{});
        m_data.unlocked_upgrades = j.value("unlocked_upgrades", std::vector<std::string>{});
        m_data.equipped_skills = j.value("equipped_skills", std::vector<std::string>{});
        m_data.blood_counter = j.value("blood_counter", 0);
        m_data.mercy_counter = j.value("mercy_counter", 0);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SaveManager] Load error: " << e.what() << std::endl;
        return false;
    }
}

bool SaveManager::save() {
    nlohmann::json j;
    j["solis_sparks"] = m_data.solis_sparks;
    j["empathy_level"] = m_data.empathy_level;
    j["unlocked_lore"] = m_data.unlocked_lore;
    j["unlocked_upgrades"] = m_data.unlocked_upgrades;
    j["equipped_skills"] = m_data.equipped_skills;
    j["blood_counter"] = m_data.blood_counter;
    j["mercy_counter"] = m_data.mercy_counter;

    std::ofstream file(constants::SAVE_FILE_PATH);
    if (!file.is_open()) {
        std::cerr << "[SaveManager] Failed to open save file for writing: " << constants::SAVE_FILE_PATH << std::endl;
        return false;
    }

    file << j.dump(4);
    file.close();
    
    std::cout << "[SaveManager] Game saved successfully to " << constants::SAVE_FILE_PATH << std::endl;
    return true;
}

} // namespace solis
