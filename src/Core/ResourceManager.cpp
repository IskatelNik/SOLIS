#include "Core/ResourceManager.h"
#include <iostream>
#include <stdexcept>

namespace solis {

/**
 * Возвращает единственный экземпляр менеджера ресурсов (Singleton).
 */
ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

/**
 * Загружает шрифт из файла и сохраняет его под указанным именем.
 * @return true при успешной загрузке.
 */
bool ResourceManager::loadFont(const std::string& name, const std::string& path) {
    sf::Font font;
    if (!font.openFromFile(path)) {
        std::cerr << "[ResourceManager] Failed to load font: " << path << std::endl;
        return false;
    }
    m_fonts[name] = std::move(font);
    return true;
}

/**
 * Возвращает ссылку на загруженный шрифт.
 * @throw std::runtime_error если шрифт с таким именем не найден.
 */
const sf::Font& ResourceManager::getFont(const std::string& name) const {
    auto it = m_fonts.find(name);
    if (it == m_fonts.end()) {
        throw std::runtime_error("[ResourceManager] Font not found: " + name);
    }
    return it->second;
}

} // namespace solis
