#ifndef RUN_MANAGER_H
#define RUN_MANAGER_H

#include "Managers/DataManager.h"
#include <vector>

namespace solis {

class RunManager {
public:
    static RunManager& getInstance();

    void startNewRun();
    
    // Генерирует 1-3 варианта дверей на основе текущего уровня
    std::vector<Room> getNextRoomOptions();
    
    void moveToRoom(const Room& room);

    int getCurrentLevel() const { return m_currentLevel; }
    int getCurrentRoomIndex() const { return m_currentRoomIndex; }

private:
    RunManager() = default;
    ~RunManager() = default;
    RunManager(const RunManager&) = delete;
    RunManager& operator=(const RunManager&) = delete;

    int m_currentLevel = 1;
    int m_currentRoomIndex = 0;
};

} // namespace solis

#endif // RUN_MANAGER_H
