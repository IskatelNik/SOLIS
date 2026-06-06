#include "Managers/DataManager.h"
#include "Entities/Player.h"
#include <vector>
#include <memory>

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

    Player& getPlayer() { return m_player; }

private:
    RunManager();
    ~RunManager() = default;
    RunManager(const RunManager&) = delete;
    RunManager& operator=(const RunManager&) = delete;

    int m_currentLevel = 1;
    int m_currentRoomIndex = 0;
    Player m_player;
};

} // namespace solis
