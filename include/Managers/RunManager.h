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
    
    // MVP 3: Lore & Empathy
    void unlockLore(const std::string& id);
    bool isLoreUnlocked(const std::string& id) const;
    const std::vector<std::string>& getUnlockedLore() const { return m_unlockedLore; }

    int getEmpathyForTrait(Trait t) const;
    void addEmpathy(int amount);
    int getTotalEmpathy() const { return m_totalEmpathy; }

private:
    RunManager();
    ~RunManager() = default;
    RunManager(const RunManager&) = delete;
    RunManager& operator=(const RunManager&) = delete;

    int m_currentLevel = 1;
    int m_currentRoomIndex = 0;
    Player m_player;

    std::vector<std::string> m_unlockedLore;
    int m_totalEmpathy = 0;
};

} // namespace solis
