#include "Managers/RunManager.h"
#include "Managers/DataManager.h"
#include "Managers/SaveManager.h"
#include "Utils/GameConstans.h"
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
    
    const SaveData& save = SaveManager::getInstance().getData();
    m_player = Player();
    
    // 1. Применяем ПАССИВНЫЕ улучшения
    int hpBonus = 0;
    int empathyBonus = 0;
    const auto& upgradeDb = DataManager::getInstance().getUpgrades();

    for (const auto& uid : save.unlocked_upgrades) {
        if (upgradeDb.count(uid)) {
            const auto& u = upgradeDb.at(uid);
            if (u.type == "passive_stat") {
                if (u.stat_type == "hp_boost") hpBonus += (int)u.value;
                else if (u.stat_type == "empathy_bonus") empathyBonus += (int)u.value;
            }
        }
    }
    
    m_player.setMaxHp(constants::PLAYER_DEFAULT_MAX_HP + hpBonus);
    m_player.setEmpathy(save.empathy_level + empathyBonus);

    // 2. Восстанавливаем открытый Лор
    m_unlockedLore = save.unlocked_lore;
    
    // 3. Экипируем АКТИВНЫЕ навыки
    const auto& skillDb = DataManager::getInstance().getSkills();
    
    // Если список пуст, даем базовый скилл (Solar Flare)
    if (save.equipped_skills.empty()) {
        if (skillDb.count("skill_solar_flare")) {
            m_player.addSkill(skillDb.at("skill_solar_flare"));
        }
    } else {
        for (const auto& sid : save.equipped_skills) {
            if (skillDb.count(sid)) {
                m_player.addSkill(skillDb.at(sid));
            }
        }
    }
}

void RunManager::addSparks(int amount) {
    SaveManager::getInstance().getData().solis_sparks += amount;
    SaveManager::getInstance().save();
}

int RunManager::getTotalSparks() const {
    return SaveManager::getInstance().getData().solis_sparks;
}

void RunManager::unlockLore(const std::string& id) {
    if (!isLoreUnlocked(id)) {
        m_unlockedLore.push_back(id);
        SaveManager::getInstance().getData().unlocked_lore.push_back(id);
        SaveManager::getInstance().save();
    }
}

bool RunManager::isLoreUnlocked(const std::string& id) const {
    return std::find(m_unlockedLore.begin(), m_unlockedLore.end(), id) != m_unlockedLore.end();
}

void RunManager::addEmpathy(int amount) {
    m_player.addEmpathy(amount);
    // Сразу фиксируем в глобальном сохранении
    SaveManager::getInstance().getData().empathy_level = m_player.getEmpathy();
    SaveManager::getInstance().save();
}

int RunManager::getIdeologyScore() const {
    return SaveManager::getInstance().getData().ideology_score;
}

void RunManager::modifyIdeologyScore(int amount) {
    SaveManager::getInstance().getData().ideology_score += amount;
    SaveManager::getInstance().save();
}

int RunManager::getEventProgress(int level) const {
    auto& ep = SaveManager::getInstance().getData().event_progress;
    if (ep.find(level) != ep.end()) {
        return ep[level];
    }
    return 0;
}

void RunManager::incrementEventProgress(int level) {
    SaveManager::getInstance().getData().event_progress[level]++;
    SaveManager::getInstance().save();
}

void RunManager::incrementBloodCounter() {
    SaveManager::getInstance().getData().blood_counter++;
    SaveManager::getInstance().save();
}

void RunManager::incrementMercyCounter() {
    SaveManager::getInstance().getData().mercy_counter++;
    SaveManager::getInstance().save();
}

void RunManager::advanceLevel() {
    m_currentLevel++;
    m_currentRoomIndex = 0;
}

std::vector<Room> RunManager::getNextRoomOptions() {
    // Если игрок прошел достаточно комнат (например, 5 для MVP 5), генерируем босса
    if (m_currentRoomIndex >= 5) {
        Room bossRoom;
        bossRoom.id = "room_boss_" + std::to_string(m_currentLevel);
        bossRoom.level = m_currentLevel;
        bossRoom.type = "boss";
        bossRoom.preview_text = "Огромные Врата (БОСС)";
        bossRoom.description = "За этими вратами скрывается страж этого сектора. Пути назад нет.";
        // В зависимости от уровня ставим нужного босса. Для MVP 5 пока Виндикт на всех уровнях или только на 4.
        // Пока хардкодим Виндикта для тестирования
        bossRoom.possible_enemies = {"boss_vindict"}; 
        return { bossRoom };
    }

    const auto& allRooms = DataManager::getInstance().getRooms();
    std::vector<Room> availableRooms;

    for (const auto& room : allRooms) {
        if (room.level == m_currentLevel && room.type != "boss") {
            availableRooms.push_back(room);
        }
    }

    if (availableRooms.empty()) return {};

    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::shuffle(availableRooms.begin(), availableRooms.end(), gen);

    size_t count = std::min({static_cast<size_t>(constants::MAX_ROOM_OPTIONS), availableRooms.size()});
    return std::vector<Room>(availableRooms.begin(), availableRooms.begin() + count);
}

void RunManager::moveToRoom(const Room& room) {
    m_currentRoomIndex++;
}

} // namespace solis
