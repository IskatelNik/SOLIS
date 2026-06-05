#ifndef UI_BOX_H
#define UI_BOX_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

namespace solis {

class UIBox {
public:
    UIBox(sf::Vector2f position, sf::Vector2f size, sf::Color bgColor, sf::Color outlineColor, float outlineThickness);

    void setText(const sf::String& text, const sf::Font& font, unsigned int charSize, sf::Color textColor);
    void render(sf::RenderWindow& window);

    private:
    void wrapText(const sf::String& text, const sf::Font& font, unsigned int charSize);

    sf::RectangleShape m_shape;
    std::unique_ptr<sf::Text> m_text;
    std::string m_wrappedString;
    sf::Vector2f m_padding = {10.f, 10.f};
};

} // namespace solis

#endif // UI_BOX_H
