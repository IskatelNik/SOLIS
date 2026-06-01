#ifndef IUIELEMENT_H
#define IUIELEMENT_H

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

// Forward declarations (чтобы не тянуть тяжелые хедеры)
class EventBus;
class ResourceManager;

/**
 * @brief Базовый абстрактный интерфейс для всех элементов UI.
 */
class IUIElement {
public:
    virtual ~IUIElement() = default;

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;

    /**
     * @brief Обработка системных событий.
     *
     * @note ДЛЯ АГЕНТОВ: В SFML 3 для проверки наведения мыши используйте:
     * getBounds().contains(sf::Vector2f({static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)}))
     * Координаты мыши (mousePos) обычно приходят как sf::Vector2i в событиях мыши,
     * поэтому необходимо приведение типов и инициализация через фигурные скобки {}.
     *
     * @return true, если элемент перехватил клик/событие, иначе false.
     */
    virtual bool handleInput(const sf::Event& event) = 0;

    virtual void setPosition(sf::Vector2f pos) = 0;
    virtual sf::Vector2f getPosition() const = 0;
    virtual sf::FloatRect getBounds() const = 0;
};

/**
 * @brief Промежуточный базовый класс для избежания дублирования кода позиционирования.
 * Хранит зависимости и позицию.
 */
class UIElementBase : public IUIElement {
public:
    UIElementBase(EventBus& bus, ResourceManager& resourceManager);
    virtual ~UIElementBase() = default;

    void setPosition(sf::Vector2f pos) override;
    sf::Vector2f getPosition() const override;

    // getBounds() остаётся чисто виртуальным, так как размер
    // зависит от конкретного элемента (Sprite, Text, RectangleShape и т.д.)

protected:
    EventBus& m_bus;
    ResourceManager& m_resourceManager;
    sf::Vector2f m_position;
};

#endif // IUIELEMENT_H