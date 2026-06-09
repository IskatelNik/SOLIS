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
    void advanceLevel();

    int getCurrentLevel() const { return m_currentLevel; }
    void setCurrentLevel(int level) { m_currentLevel = level; }
    int getCurrentRoomIndex() const { return m_currentRoomIndex; }

    Player& getPlayer() { return m_player; }
    
    // MVP 3 & 4 & 5: Lore, Empathy, Sparks, Ideology, Events
    void unlockLore(const std::string& id);
    bool isLoreUnlocked(const std::string& id) const;
    const std::vector<std::string>& getUnlockedLore() const { return m_unlockedLore; }

    void addEmpathy(int amount);
    void addSparks(int amount);
    int getTotalSparks() const;

    int getIdeologyScore() const;
    void modifyIdeologyScore(int amount);

    int getEventProgress(int level) const;
    void incrementEventProgress(int level);

    void incrementBloodCounter();
    void incrementMercyCounter();

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
