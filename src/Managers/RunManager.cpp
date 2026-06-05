#include "Managers/RunManager.h"
#include <algorithm>
#include <random>

namespace solis {

RunManager& RunManager::getInstance() {
    static RunManager instance;
    return instance;
}

void RunManager::startNewRun() {
    m_currentLevel = 1;
    m_currentRoomIndex = 0;
}

std::vector<Room> RunManager::getNextRoomOptions() {
    const auto& allRooms = DataManager::getInstance().getRooms();
    std::vector<Room> availableRooms;

    // Фильтруем комнаты по текущему уровню
    for (const auto& room : allRooms) {
        if (room.level == m_currentLevel) {
            availableRooms.push_back(room);
        }
    }

    if (availableRooms.empty()) return {};

    // Случайный выбор от 1 до 3 комнат
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::shuffle(availableRooms.begin(), availableRooms.end(), gen);

    size_t count = std::min({static_cast<size_t>(3), availableRooms.size()});
    return std::vector<Room>(availableRooms.begin(), availableRooms.begin() + count);
}

void RunManager::moveToRoom(const Room& room) {
    m_currentRoomIndex++;
    // В будущем здесь будет переход между уровнями при достижении maxRoomsPerLevel
}

} // namespace solis
