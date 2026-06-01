#include "UI/IUIElement.h"

UIElementBase::UIElementBase(EventBus& bus, ResourceManager& resourceManager)
    : m_bus(bus)
    , m_resourceManager(resourceManager)
    , m_position({ 0.f, 0.f }) // SFML 3: Инициализация вектора через фигурные скобки
{
}

void UIElementBase::setPosition(sf::Vector2f pos) {
    m_position = pos;
}

sf::Vector2f UIElementBase::getPosition() const {
    return m_position;
}