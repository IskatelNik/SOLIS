#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include "SaveProfile.h"
#include <filesystem>

class SaveManager {
public:
    SaveManager() = default;
    ~SaveManager() = default;

    // Запрещаем копирование, так как менеджер должен быть единственным (управляется Core/Game)
    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;

    // Управление файлами
    bool loadProfile(const std::filesystem::path& filepath);
    bool saveProfile(const std::filesystem::path& filepath) const;

    // Доступ к данным
    SaveProfile& getProfile();
    const SaveProfile& getProfile() const;

    // Сброс прогресса
    void resetProfile();

private:
    SaveProfile m_currentProfile;
};

#endif // SAVE_MANAGER_H