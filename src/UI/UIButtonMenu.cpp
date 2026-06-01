#include "UI/UIButtonMenu.h"
#include <cmath>

UIButtonMenu::UIButtonMenu(const sf::Font& font, sf::Vector2f startPosition)
    : m_font(font), m_position(startPosition),
    m_normalColor(sf::Color(40, 40, 40, 200)),
    m_hoverColor(sf::Color(80, 80, 80, 255)),
    m_textColor(sf::Color::White)
{
}

void UIButtonMenu::setOptions(const std::vector<std::string>& labels, std::function<void(int)> onSelect) {
    m_buttons.clear();
    m_onSelect = onSelect;

    float currentY = m_position.y;

    for (const auto& label : labels) {
        // Вызываем наш новый конструктор, передавая шрифт
        Button btn(m_font);

        btn.label = label;

        // btn.text уже создана со шрифтом, просто настраиваем её
        btn.text.setString(label);
        btn.text.setCharacterSize(22);
        btn.text.setFillColor(m_textColor);

        // Округляем позиции для четкости
        float textWidth = btn.text.getLocalBounds().size.x;
        float textHeight = btn.text.getLocalBounds().size.y;

        btn.rect.setSize(sf::Vector2f({ textWidth + 40.0f, textHeight + 20.0f }));
        btn.rect.setPosition(sf::Vector2f({ std::round(m_position.x), std::round(currentY) }));
        btn.rect.setFillColor(m_normalColor);
        btn.rect.setOutlineColor(sf::Color(100, 100, 100));
        btn.rect.setOutlineThickness(1.0f);

        // Центрируем текст внутри кнопки
        float textX = m_position.x + 20.0f;
        float textY = currentY + 5.0f; // Небольшой оффсет сверху для шрифтов
        btn.text.setPosition(sf::Vector2f({ std::round(textX), std::round(textY) }));

        m_buttons.push_back(std::move(btn));

        currentY += btn.rect.getSize().y + 15.0f; // Отступ между кнопками
    }
}

void UIButtonMenu::update(float /*dt*/) {
    // Анимация Hover (изменение цвета)
    for (auto& btn : m_buttons) {
        if (btn.isHovered) {
            btn.rect.setFillColor(m_hoverColor);
        }
        else {
            btn.rect.setFillColor(m_normalColor);
        }
    }
}

void UIButtonMenu::draw(sf::RenderTarget& target) const {
    for (const auto& btn : m_buttons) {
        target.draw(btn.rect);
        target.draw(btn.text);
    }
}

bool UIButtonMenu::handleInput(const sf::Event& event) {
    if (m_buttons.empty()) return false;

    // SFML 3: Обработка движения мыши
    if (const auto* mouseMove = event.getIf<sf::Event::MouseMoved>()) {
        sf::Vector2f mousePos({ static_cast<float>(mouseMove->position.x), static_cast<float>(mouseMove->position.y) });
        bool isHandled = false;

        for (auto& btn : m_buttons) {
            bool wasHovered = btn.isHovered;
            btn.isHovered = btn.rect.getGlobalBounds().contains(mousePos);
            if (btn.isHovered) isHandled = true;
        }
        return isHandled;
    }

    // SFML 3: Обработка клика
    if (const auto* mouseClick = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseClick->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos({ static_cast<float>(mouseClick->position.x), static_cast<float>(mouseClick->position.y) });

            for (size_t i = 0; i < m_buttons.size(); ++i) {
                if (m_buttons[i].rect.getGlobalBounds().contains(mousePos)) {
                    if (m_onSelect) {
                        m_onSelect(static_cast<int>(i));
                    }
                    return true;
                }
            }
        }
    }

    return false;
}

void UIButtonMenu::setPosition(sf::Vector2f pos) {
    float diffY = pos.y - m_position.y;
    float diffX = pos.x - m_position.x;
    m_position = pos;

    // Смещаем все существующие кнопки относительно новой стартовой позиции
    for (auto& btn : m_buttons) {
        btn.rect.move(sf::Vector2f({ diffX, diffY }));
        btn.text.move(sf::Vector2f({ diffX, diffY }));
    }
}

sf::Vector2f UIButtonMenu::getPosition() const {
    return m_position;
}

sf::FloatRect UIButtonMenu::getBounds() const {
    if (m_buttons.empty()) {
        return sf::FloatRect({ m_position.x, m_position.y }, { 0.0f, 0.0f });
    }

    float totalHeight = 0.0f;
    float maxWidth = 0.0f;
    for (const auto& btn : m_buttons) {
        totalHeight += btn.rect.getSize().y + 15.0f; // с учетом отступа
        if (btn.rect.getSize().x > maxWidth) {
            maxWidth = btn.rect.getSize().x;
        }
    }
    return sf::FloatRect({ m_position.x, m_position.y }, { maxWidth, totalHeight - 15.0f });
}