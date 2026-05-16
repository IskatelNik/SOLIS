#include "Data/DataLoader.h"
#include <fstream>
#include <iostream>

bool DataLoader::loadAllData(const std::filesystem::path& dataDirectory) {
    try {
        // Загрузка врагов
        std::filesystem::path enemiesPath = dataDirectory / "enemies.json";
        std::ifstream enemiesFile(enemiesPath);
        if (!enemiesFile.is_open()) {
            std::cerr << "[DataLoader] Error: Cannot open " << enemiesPath << "\n";
            return false;
        }

        nlohmann::json enemiesJson;
        enemiesFile >> enemiesJson;
        for (const auto& item : enemiesJson.at("enemies")) {
            auto enemy = item.get<EnemyDef>();
            m_enemies[enemy.id] = enemy;
        }

        // Загрузка лора
        std::filesystem::path lorePath = dataDirectory / "lore.json";
        std::ifstream loreFile(lorePath);
        if (!loreFile.is_open()) {
            std::cerr << "[DataLoader] Error: Cannot open " << lorePath << "\n";
            return false;
        }

        nlohmann::json loreJson;
        loreFile >> loreJson;
        for (const auto& item : loreJson.at("lore")) {
            auto lorePiece = item.get<LoreDef>();
            m_lore[lorePiece.id] = lorePiece;

            // Заполняем кэш для O(1) поиска по черте
            m_traitToLoreIds[lorePiece.associatedTrait].push_back(lorePiece.id);
        }

        // Опционально: загрузка комнат (если файл существует)
        std::filesystem::path roomsPath = dataDirectory / "rooms.json";
        std::ifstream roomsFile(roomsPath);
        if (roomsFile.is_open()) {
            nlohmann::json roomsJson;
            roomsFile >> roomsJson;
            for (const auto& item : roomsJson.at("rooms")) {
                auto room = item.get<RoomDef>();
                m_rooms[room.id] = room;
            }
        }
        else {
            std::cout << "[DataLoader] Warning: rooms.json not found, skipping.\n";
        }

    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "[DataLoader] JSON Parse Error: " << e.what() << "\n";
        return false;
    }
    catch (const nlohmann::json::type_error& e) {
        std::cerr << "[DataLoader] JSON Type Error (mismatched data types): " << e.what() << "\n";
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "[DataLoader] General Error: " << e.what() << "\n";
        return false;
    }

    std::cout << "[DataLoader] Successfully loaded data.\n";
    std::cout << "  - Enemies: " << m_enemies.size() << "\n";
    std::cout << "  - Lore: " << m_lore.size() << "\n";
    return true;
}

const EnemyDef& DataLoader::getEnemy(const std::string& id) const {
    auto it = m_enemies.find(id);
    if (it == m_enemies.end()) {
        throw std::runtime_error("[DataLoader] Enemy ID not found: " + id);
    }
    return it->second;
}

const LoreDef& DataLoader::getLore(const std::string& id) const {
    auto it = m_lore.find(id);
    if (it == m_lore.end()) {
        throw std::runtime_error("[DataLoader] Lore ID not found: " + id);
    }
    return it->second;
}

std::vector<const LoreDef*> DataLoader::getLoreForTrait(const std::string& traitId) const {
    std::vector<const LoreDef*> result;

    auto it = m_traitToLoreIds.find(traitId);
    if (it != m_traitToLoreIds.end()) {
        // Если черта найдена, резервируем место и собираем указатели
        const auto& loreIds = it->second;
        result.reserve(loreIds.size());
        for (const auto& id : loreIds) {
            // Безопасно используем at(), так как мы точно знаем, что ID есть в m_lore
            result.push_back(&m_lore.at(id));
        }
    }

    return result;
}

const std::unordered_map<std::string, LoreDef>& DataLoader::getAllLore() const {
    return m_lore;
}