#include "Managers/RunManager.h"
#include "Managers/DataManager.h"
#include "Managers/SaveManager.h"
#include "Utils/GameConstans.h"
#include <algorithm>
#include <random>

namespace solis {

/**
 * @brief Синглтон для управления состоянием текущего забега.
 */
RunManager& RunManager::getInstance() {
    static RunManager instance;
    return instance;
}

RunManager::RunManager() {}

/**
 * @brief Инициализирует новый забег, применяя все купленные улучшения и выбранные навыки.
 */
void RunManager::startNewRun() {
    m_currentLevel = 1;
    m_currentRoomIndex = 0;
    
    const SaveData& save = SaveManager::getInstance().getData();
    m_player = Player();
    
    // Применение ПАССИВНЫХ бонусов от улучшений Хаба
    int hpBonus = 0;
    float overloadBonus = 0.0f;
    int damageBonus = 0;
    int empathyBonus = 0;
    float heatGainMult = 1.0f;

    const auto& upgradeDb = DataManager::getInstance().getUpgrades();

    for (const auto& uid : save.unlocked_upgrades) {
        if (upgradeDb.count(uid)) {
            const auto& u = upgradeDb.at(uid);
            if (u.type == "passive_stat") {
                if (u.effect_target == "max_hp") hpBonus += (int)u.effect_value;
                else if (u.effect_target == "overload_threshold") overloadBonus += u.effect_value;
                else if (u.effect_target == "base_damage") damageBonus += (int)u.effect_value;
                else if (u.effect_target == "empathy_base") empathyBonus += (int)u.effect_value;
                else if (u.effect_target == "heat_gain_multiplier") heatGainMult *= u.effect_value;
            }
        }
    }
    
    m_player.setMaxHp(constants::PLAYER_DEFAULT_MAX_HP + hpBonus);
    m_player.setBaseOverloadThreshold(constants::HEAT_OVERLOAD_THRESHOLD + overloadBonus);
    m_player.setBaseDamageBonus(damageBonus);
    m_player.setEmpathy(save.empathy_level + empathyBonus);
    m_player.setBaseHeatGainMultiplier(heatGainMult);

    // Восстановление открытого Лора для использования в диалогах
    m_unlockedLore = save.unlocked_lore;
    
    // Экипировка АКТИВНЫХ навыков из Хаба
    const auto& skillDb = DataManager::getInstance().getSkills();
    
    if (save.equipped_skills.empty()) {
        // Если ничего не выбрано — даем базовую вспышку
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

/**
 * @brief Разблокирует новую запись лора в архиве игрока.
 */
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

/**
 * @brief Увеличивает уровень эмпатии игрока (постоянная характеристика).
 */
void RunManager::addEmpathy(int amount) {
    m_player.addEmpathy(amount);
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

/**
 * @brief Переход на следующий биом (уровень).
 */
void RunManager::advanceLevel() {
    m_currentLevel++;
    m_currentRoomIndex = 0;
}

/**
 * @brief Генерирует варианты комнат для следующего шага игрока.
 * После 12-й комнаты гарантированно выдает босса.
 */
std::vector<Room> RunManager::getNextRoomOptions() {
    // Проверка порога для появления босса уровня
    if (m_currentRoomIndex >= 12) {
        Room bossRoom;
        bossRoom.id = "room_boss_" + std::to_string(m_currentLevel);
        bossRoom.level = m_currentLevel;
        bossRoom.type = "boss";
        
        // Определение босса и атмосферного описания для каждого биома
        if (m_currentLevel == 1) {
            bossRoom.possible_enemies = {"boss_libert"};
            bossRoom.preview_text = "Укрепленные врата карцера, из-за двери слышен лязг офицерской стали";
            bossRoom.description = "Просторный зал управления тюрьмой. Выходы заблокированы тяжелыми решетками. Генерал Либерт лично преграждает вам путь к свободе. Весы должны снова выровняться...";
        }
        else if (m_currentLevel == 2) {
            bossRoom.possible_enemies = {"boss_divit"};
            bossRoom.preview_text = "Позолоченные двери лифта, cлышен звон монет и прерывистое дыхание";
            bossRoom.description = "Роскошный кабинет на верхнем ярусе мануфактуры. Владелец заводов, барон Дивит, нервно сжимает свое оружие, прячась за роскошью, облитой чужой кровью. Казна должна снова идти на благо людей...";
        }
        else if (m_currentLevel == 3) {
            bossRoom.possible_enemies = {"boss_mutat"};
            bossRoom.preview_text = "Герметичный шлюз, из него сочится ослепительный белый свет и резкий запах химикатов";
            bossRoom.description = "Главная операционная лабораторий. В центре зала возвышается доктор Мутат. Его глаза безумно блестят в предвкушении идеального образца для самого грандиозного эксперимента. Алхимия должна создавать а не уничтожать...";
        }
        else if (m_currentLevel == 4) {
            bossRoom.possible_enemies = {"boss_vindict"};
            bossRoom.preview_text = "Раскаленные врата Ядра, ваше тело разрывается от оглушительного рева пламени и невыносимого жара";
            bossRoom.description = "Самое сердце великой Печи. Лорд Виндикт стоит в центре ядра, в его глазах горит безумное пламя которое не потушить вашей кровью. Солис должен снова загореться...";
        }
        else {
            bossRoom.possible_enemies = {"boss_vindict"};
            bossRoom.description = "За этими вратами скрывается страж этого сектора. Пути назад нет.";
        }
        
        return { bossRoom };
    }

    // Случайная выборка обычных комнат текущего уровня
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
