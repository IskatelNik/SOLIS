#include "Managers/RunManager.h"
#include "Managers/DataManager.h"
#include <algorithm>
#include <random>

namespace solis {

RunManager& RunManager::getInstance() {
    static RunManager instance;
    return instance;
}

RunManager::RunManager() {}

void RunManager::startNewRun() {
    m_currentLevel = 1;
    m_currentRoomIndex = 0;
    
    // Сохраняем эмпатию перед сбросом игрока (пока нет SaveManager)
    int oldEmpathy = m_player.getEmpathy();
    
    // Знания (Лор) теперь также перманентны, как и Эмпатия. 
    // НЕ вызываем m_unlockedLore.clear(), чтобы игрок не терял прогресс.

    m_player = Player();
    m_player.setEmpathy(oldEmpathy);
    
    // В MVP 2-3 даем игроку все загруженные навыки для теста
    for (auto const& [id, skill] : DataManager::getInstance().getSkills()) {
        m_player.addSkill(skill);
    }
}

void RunManager::unlockLore(const std::string& id) {
    if (!isLoreUnlocked(id)) {
        m_unlockedLore.push_back(id);
    }
}

bool RunManager::isLoreUnlocked(const std::string& id) const {
    return std::find(m_unlockedLore.begin(), m_unlockedLore.end(), id) != m_unlockedLore.end();
}

void RunManager::addEmpathy(int amount) {
    m_totalEmpathy += amount;
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
