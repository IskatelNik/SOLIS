#include "Data/ResourceManager.h"
#include <iostream>

bool ResourceManager::loadFont(const std::string& id, const std::filesystem::path& filepath) {
    // Если шрифт с таким ID уже существует, игнорируем запрос и возвращаем true
    if (m_fonts.find(id) != m_fonts.end()) {
        return true;
    }

    // В SFML 3 для шрифтов используется openFromFile, а не loadFromFile
    if (!m_fonts[id].openFromFile(filepath)) {
        std::cerr << "[ResourceManager] Error: Failed to open font '" << id
            << "' from path: " << filepath << std::endl;
        m_fonts.erase(id); // Удаляем пустой объект в случае неудачи
        return false;
    }

    return true;
}

bool ResourceManager::loadTexture(const std::string& id, const std::filesystem::path& filepath) {
    if (m_textures.find(id) != m_textures.end()) {
        return true;
    }

    // Текстуры по-прежнему загружаются через loadFromFile
    if (!m_textures[id].loadFromFile(filepath)) {
        std::cerr << "[ResourceManager] Error: Failed to load texture '" << id
            << "' from path: " << filepath << std::endl;
        m_textures.erase(id);
        return false;
    }

    return true;
}

bool ResourceManager::loadShader(const std::string& id, const std::filesystem::path& filepath, sf::Shader::Type type) {
    if (m_shaders.find(id) != m_shaders.end()) {
        return true;
    }

    // Проверка поддержки шейдеров на уровне видеокарты пользователя
    if (!sf::Shader::isAvailable()) {
        std::cerr << "[ResourceManager] Error: Shaders are not available on this system!" << std::endl;
        return false;
    }

    if (!m_shaders[id].loadFromFile(filepath, type)) {
        std::cerr << "[ResourceManager] Error: Failed to load shader '" << id
            << "' from path: " << filepath << std::endl;
        m_shaders.erase(id);
        return false;
    }

    return true;
}

const sf::Font& ResourceManager::getFont(const std::string& id) const {
    auto it = m_fonts.find(id);
    if (it == m_fonts.end()) {
        throw std::runtime_error("Font not found: " + id);
    }
    return it->second;
}

const sf::Texture& ResourceManager::getTexture(const std::string& id) const {
    auto it = m_textures.find(id);
    if (it == m_textures.end()) {
        throw std::runtime_error("Texture not found: " + id);
    }
    return it->second;
}

sf::Shader& ResourceManager::getShader(const std::string& id) {
    auto it = m_shaders.find(id);
    if (it == m_shaders.end()) {
        throw std::runtime_error("Shader not found: " + id);
    }
    return it->second;
}

void ResourceManager::clear() {
    m_fonts.clear();
    m_textures.clear();
    m_shaders.clear();
    std::cout << "[ResourceManager] All resources cleared." << std::endl;
}