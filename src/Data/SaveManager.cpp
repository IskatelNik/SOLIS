#include "Data/SaveManager.h"
#include <fstream>
#include <system_error>

bool SaveManager::loadProfile(const std::filesystem::path& filepath) {
    // Если файла нет (первый запуск), сбрасываем в дефолт и возвращаем false
    if (!std::filesystem::exists(filepath)) {
        resetProfile();
        return false;
    }

    std::ifstream file(filepath);
    if (!file.is_open()) {
        resetProfile();
        return false;
    }

    try {
        nlohmann::json jsonData;
        file >> jsonData;
        // Заполняем структуру данными. Отсутствующие поля получат дефолтные значения.
        m_currentProfile = jsonData.get<SaveProfile>();
        return true;
    }
    catch (const nlohmann::json::exception& /*e*/) {
        // Если файл поврежден, сбрасываем профиль до "чистого" и предотвращаем краш
        resetProfile();
        return false;
    }
}

bool SaveManager::saveProfile(const std::filesystem::path& filepath) const {
    // Подготовка временного пути
    std::filesystem::path tempFilepath = filepath;
    tempFilepath += ".tmp";

    try {
        // Конвертируем профиль в JSON
        nlohmann::json jsonData = m_currentProfile;

        std::ofstream tempFile(tempFilepath);
        if (!tempFile.is_open()) {
            return false;
        }

        // Сохраняем с отступом в 4 пробела (pretty print) для читаемости
        tempFile << jsonData.dump(4);
        tempFile.close();

        // Безопасная атомарная перезапись основного файла
        std::error_code ec;
        // В C++17 filesystem::rename корректно переписывает старый файл, если он существует
        std::filesystem::rename(tempFilepath, filepath, ec);

        if (ec) {
            // Если переименовать не удалось (например, нет прав)
            return false;
        }

        return true;
    }
    catch (const nlohmann::json::exception& /*e*/) {
        // Перехват ошибок сериализации
        return false;
    }
}

SaveProfile& SaveManager::getProfile() {
    return m_currentProfile;
}

const SaveProfile& SaveManager::getProfile() const {
    return m_currentProfile;
}

void SaveManager::resetProfile() {
    // Просто присваиваем новый "чистый" экземпляр структуры
    m_currentProfile = SaveProfile{};
}