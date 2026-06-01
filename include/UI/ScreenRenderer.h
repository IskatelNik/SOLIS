#ifndef SCREEN_RENDERER_H
#define SCREEN_RENDERER_H

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#include <optional> // ДОБАВЛЕНО для std::optional

#include "Data/ResourceManager.h"

class ScreenRenderer {
public:
    ScreenRenderer(ResourceManager& resManager, sf::Vector2u baseResolution);
    ~ScreenRenderer() = default;

    ScreenRenderer(const ScreenRenderer&) = delete;
    ScreenRenderer& operator=(const ScreenRenderer&) = delete;

    sf::RenderTarget& getTarget();
    void update(float dt);
    void clear(sf::Color color = sf::Color::Black);
    void displayToWindow(sf::RenderWindow& mainWindow);

private:
    void updateViewRatio(sf::RenderWindow& mainWindow);

    ResourceManager& m_resourceManager;

    sf::RenderTexture m_renderTexture;

    // ИСПОЛЬЗУЕМ std::optional для отложенной инициализации sf::Sprite в SFML 3
    std::optional<sf::Sprite> m_screenSprite;

    float m_shaderTime;
    bool m_shadersSupported;
    sf::Vector2u m_baseResolution;
};

#endif // SCREEN_RENDERER_H