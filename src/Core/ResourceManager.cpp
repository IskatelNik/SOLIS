#include "Core/ResourceManager.h"
#include <iostream>
#include <stdexcept>

namespace solis {

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::loadFont(const std::string& name, const std::string& path) {
    sf::Font font;
    if (!font.openFromFile(path)) {
        std::cerr << "[ResourceManager] Failed to load font: " << path << std::endl;
        return false;
    }
    m_fonts[name] = std::move(font);
    return true;
}

const sf::Font& ResourceManager::getFont(const std::string& name) const {
    auto it = m_fonts.find(name);
    if (it == m_fonts.end()) {
        throw std::runtime_error("[ResourceManager] Font not found: " + name);
    }
    return it->second;
}

} // namespace solis
