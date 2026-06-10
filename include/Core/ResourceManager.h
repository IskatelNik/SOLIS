#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>

namespace solis {

class ResourceManager {
public:
    static ResourceManager& getInstance();

    // Загрузка шрифта. Возвращает true, если успешно.
    bool loadFont(const std::string& name, const std::string& path);
    
    // Получение ссылки на шрифт. Кидает исключение или возвращает дефолтный, если не найден.
    const sf::Font& getFont(const std::string& name) const;

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::map<std::string, sf::Font> m_fonts;
};

} // namespace solis

#endif // RESOURCE_MANAGER_H
