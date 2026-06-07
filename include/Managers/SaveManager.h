#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace solis {

struct SaveData {
    int solis_sparks = 0;
    int empathy_level = 0;
    std::vector<std::string> unlocked_lore;
    std::vector<std::string> unlocked_upgrades;
    std::vector<std::string> equipped_skills;
    int blood_counter = 0;
    int mercy_counter = 0;
};

class SaveManager {
public:
    static SaveManager& getInstance();

    bool load();
    bool save();

    SaveData& getData() { return m_data; }
    const SaveData& getData() const { return m_data; }

private:
    SaveManager() = default;
    ~SaveManager() = default;
    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;

    SaveData m_data;
};

} // namespace solis

#endif // SAVE_MANAGER_H
