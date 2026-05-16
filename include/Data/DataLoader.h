#ifndef DATALOADER_HPP
#define DATALOADER_HPP

#include "DataDefs.h"
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>

class DataLoader {
public:
    DataLoader() = default;
    ~DataLoader() = default;

    // Запрещаем копирование менеджера данных
    DataLoader(const DataLoader&) = delete;
    DataLoader& operator=(const DataLoader&) = delete;

    // Инициализация
    bool loadAllData(const std::filesystem::path& dataDirectory);

    // Геттеры
    const EnemyDef& getEnemy(const std::string& id) const;
    const LoreDef& getLore(const std::string& id) const;

    // Возвращает список невладеющих указателей (наблюдателей) на объекты в памяти DataLoader
    std::vector<const LoreDef*> getLoreForTrait(const std::string& traitId) const;

    const std::unordered_map<std::string, LoreDef>& getAllLore() const;

private:
    std::unordered_map<std::string, EnemyDef> m_enemies;
    std::unordered_map<std::string, LoreDef> m_lore;
    std::unordered_map<std::string, RoomDef> m_rooms;

    // Кэш: ассоциирует конкретную черту со списком ID лора (для O(1) поиска в соц. бою)
    std::unordered_map<std::string, std::vector<std::string>> m_traitToLoreIds;
};

#endif // DATALOADER_HPP