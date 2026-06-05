#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace solis {

struct Room {
    std::string id;
    int level;
    std::string type;
    std::string preview_text;
    std::string description;
    std::vector<std::string> possible_enemies; // Для будущих MVP, но есть в структуре JSON
};

class DataManager {
public:
    static DataManager& getInstance();

    bool loadRooms(const std::string& filepath);
    const std::vector<Room>& getRooms() const { return m_rooms; }

private:
    DataManager() = default;
    ~DataManager() = default;
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    std::vector<Room> m_rooms;
};

} // namespace solis

#endif // DATA_MANAGER_H
