#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <unordered_map>
#include <string>
#include <filesystem>
#include <stdexcept>

class ResourceManager {
public:
    // Менеджер ресурсов не должен копироваться (защита от утечек и дублирования)
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    // --- Методы загрузки (Loading) ---
    bool loadFont(const std::string& id, const std::filesystem::path& filepath);
    bool loadTexture(const std::string& id, const std::filesystem::path& filepath);
    bool loadShader(const std::string& id, const std::filesystem::path& filepath, sf::Shader::Type type);

    // --- Методы получения (Getters) ---
    const sf::Font& getFont(const std::string& id) const;
    const sf::Texture& getTexture(const std::string& id) const;
    sf::Shader& getShader(const std::string& id); // Не const, так как шейдеру нужны uniform-параметры

    // --- Утилиты ---
    void clear();

private:
    std::unordered_map<std::string, sf::Font> m_fonts;
    std::unordered_map<std::string, sf::Texture> m_textures;
    std::unordered_map<std::string, sf::Shader> m_shaders;
};

#endif // !RESOURCE_MANAGER