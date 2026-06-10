#ifndef UI_BOX_H
#define UI_BOX_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "Utils/GameConstans.h"

namespace solis {

class UIBox {
public:
    UIBox(sf::Vector2f position, sf::Vector2f size, sf::Color bgColor, sf::Color outlineColor, float outlineThickness);

    void setText(const sf::String& text, const sf::Font& font, unsigned int charSize, sf::Color textColor);
    void setProgressBar(float percentage, sf::Color color);
    void render(sf::RenderWindow& window);

private:
    void wrapText(const sf::String& text, const sf::Font& font, unsigned int charSize);

    sf::RectangleShape m_shape;
    sf::RectangleShape m_bar;
    std::unique_ptr<sf::Text> m_text;
    
    sf::Vector2f m_padding = {constants::UI_PADDING, constants::UI_PADDING};
    float m_barPercentage = 0.0f;
    bool m_hasBar = false;
};

} // namespace solis

#endif // UI_BOX_H
