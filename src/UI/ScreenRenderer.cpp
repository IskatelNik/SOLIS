#include "UI/ScreenRenderer.h"
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <stdexcept>

ScreenRenderer::ScreenRenderer(ResourceManager& resManager, sf::Vector2u baseResolution)
    : m_resourceManager(resManager)
    , m_shaderTime(0.0f)
    , m_shadersSupported(false)
    , m_baseResolution(baseResolution)
{
    m_shadersSupported = sf::Shader::isAvailable();

    // Создаем/изменяем размер текстуры 
    if (!m_renderTexture.resize(m_baseResolution)) {
        // Ошибка создания
    }

    m_renderTexture.setSmooth(true);

    // ИНИЦИАЛИЗАЦИЯ SPRITE: 
    // Теперь, когда m_renderTexture готова, мы можем безопасно создать Sprite, 
    // передав ему валидную ссылку на текстуру.
    m_screenSprite.emplace(m_renderTexture.getTexture());
}

sf::RenderTarget& ScreenRenderer::getTarget() {
    return m_renderTexture;
}

void ScreenRenderer::update(float dt) {
    m_shaderTime += dt;
}

void ScreenRenderer::clear(sf::Color color) {
    m_renderTexture.clear(color);
}

void ScreenRenderer::updateViewRatio(sf::RenderWindow& mainWindow) {
    sf::Vector2u windowSize = mainWindow.getSize();

    float windowRatio = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
    float viewRatio = static_cast<float>(m_baseResolution.x) / static_cast<float>(m_baseResolution.y);

    float sizeX = 1.0f;
    float sizeY = 1.0f;
    float posX = 0.0f;
    float posY = 0.0f;

    if (windowRatio > viewRatio) {
        sizeX = viewRatio / windowRatio;
        posX = (1.0f - sizeX) / 2.0f;
    }
    else {
        sizeY = windowRatio / viewRatio;
        posY = (1.0f - sizeY) / 2.0f;
    }

    sf::View view;
    view.setSize(sf::Vector2f({ static_cast<float>(m_baseResolution.x), static_cast<float>(m_baseResolution.y) }));
    view.setCenter(sf::Vector2f({ m_baseResolution.x / 2.0f, m_baseResolution.y / 2.0f }));
    view.setViewport(sf::FloatRect({ posX, posY }, { sizeX, sizeY }));

    mainWindow.setView(view);
}

void ScreenRenderer::displayToWindow(sf::RenderWindow& mainWindow) {
    m_renderTexture.display();
    updateViewRatio(mainWindow);

    // ОБНОВЛЕНИЕ ТЕКСТУРЫ: Обращаемся к optional через оператор ->
    m_screenSprite->setTexture(m_renderTexture.getTexture());

    if (m_shadersSupported) {
        try {
            sf::Shader& crtShader = m_resourceManager.getShader("crt");
            crtShader.setUniform("u_time", m_shaderTime);

            sf::RenderStates states;
            states.shader = &crtShader;

            // ОТРИСОВКА: Разыменовываем optional через *
            mainWindow.draw(*m_screenSprite, states);

        }
        catch (const std::exception&) {
            mainWindow.draw(*m_screenSprite);
        }
    }
    else {
        mainWindow.draw(*m_screenSprite);
    }
}