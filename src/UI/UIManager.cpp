#include "UI/UIManager.h"

void UIManager::clear() {
    m_elements.clear();
}

void UIManager::updateAll(float dt) {
    for (auto& element : m_elements) {
        element->update(dt);
    }
}

void UIManager::drawAll(sf::RenderTarget& target) const {
    // Отрисовываем от старых элементов к новым (нижние слои -> верхние слои)
    for (const auto& element : m_elements) {
        element->draw(target);
    }
}

bool UIManager::handleInputAll(const sf::Event& event) {
    // Итерируемся в обратном порядке (от верхних к нижним).
    // Если кнопка лежит поверх другой кнопки, она перехватит клик первой и вернет true.
    for (auto it = m_elements.rbegin(); it != m_elements.rend(); ++it) {
        if ((*it)->handleInput(event)) {
            return true; // Клик успешно перехвачен, прерываем цикл
        }
    }
    return false; // Ни один элемент не отреагировал
}