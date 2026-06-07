#ifndef RUN_MANAGER_H
#define RUN_MANAGER_H

#include "Managers/DataManager.h"
#include "Entities/Player.h"
#include "Managers/SaveManager.h"
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
    
    // MVP 3 & 4: Lore, Empathy, Sparks
    void unlockLore(const std::string& id);
    bool isLoreUnlocked(const std::string& id) const;
    const std::vector<std::string>& getUnlockedLore() const { return m_unlockedLore; }

    void addEmpathy(int amount);
    void addSparks(int amount);
    int getTotalSparks() const;

private:
    RunManager();
    ~RunManager() = default;
    RunManager(const RunManager&) = delete;
    RunManager& operator=(const RunManager&) = delete;

    int m_currentLevel = 1;
    int m_currentRoomIndex = 0;
    Player m_player;

    std::vector<std::string> m_unlockedLore;
};

} // namespace solis

#endif // RUN_MANAGER_H
