#ifndef UIBUTTONMENU_H
#define UIBUTTONMENU_H

#include "IUIElement.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <vector>
#include <string>
#include <functional>

class UIButtonMenu : public IUIElement {
public:
    UIButtonMenu(const sf::Font& font, sf::Vector2f startPosition);
    ~UIButtonMenu() override = default;

    void setOptions(const std::vector<std::string>& labels, std::function<void(int)> onSelect);

    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;
    bool handleInput(const sf::Event& event) override;

    void setPosition(sf::Vector2f pos) override;
    sf::Vector2f getPosition() const override;
    sf::FloatRect getBounds() const override;

private:
    struct Button {
        sf::RectangleShape rect;
        sf::Text text;
        bool isHovered = false;
        std::string label;

        // Явно инициализируем rect и text, так как в SFML 3 нет default-конструкторов
        explicit Button(const sf::Font& font)
            : rect(sf::Vector2f({ 0.0f, 0.0f })), text(font) {
        }
    };

    const sf::Font& m_font;
    sf::Vector2f m_position;

    std::vector<Button> m_buttons;
    std::function<void(int)> m_onSelect;

    sf::Color m_normalColor;
    sf::Color m_hoverColor;
    sf::Color m_textColor;
};

#endif // UIBUTTONMENU_H