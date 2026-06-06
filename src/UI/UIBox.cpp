#include "UI/UIBox.h"

namespace solis {

UIBox::UIBox(sf::Vector2f position, sf::Vector2f size, sf::Color bgColor, sf::Color outlineColor, float outlineThickness) {
    m_shape.setPosition(position);
    m_shape.setSize(size);
    m_shape.setFillColor(bgColor);
    m_shape.setOutlineColor(outlineColor);
    m_shape.setOutlineThickness(outlineThickness);
}

void UIBox::setText(const sf::String& text, const sf::Font& font, unsigned int charSize, sf::Color textColor) {
    if (!m_text) {
        m_text = std::make_unique<sf::Text>(font);
    } else {
        m_text->setFont(font);
    }
    m_text->setCharacterSize(charSize);
    m_text->setFillColor(textColor);
    wrapText(text, font, charSize);
}

void UIBox::wrapText(const sf::String& text, const sf::Font& font, unsigned int charSize) {
    if (!m_text) return;

    sf::String currentLine;
    sf::String result;
    float maxWidth = m_shape.getSize().x - (m_padding.x * 2.f);

    sf::Text tempText(font, "", charSize);

    for (std::uint32_t c : text) {
        sf::String charStr(static_cast<char32_t>(c));
        if (c == '\n') {
            result += currentLine + "\n";
            currentLine = "";
            continue;
        }

        tempText.setString(currentLine + charStr);
        if (tempText.getLocalBounds().size.x > maxWidth) {
            size_t lastSpace = std::string::npos;
            for (size_t i = 0; i < currentLine.getSize(); ++i) {
                if (currentLine[i] == ' ') lastSpace = i;
            }

            if (lastSpace != std::string::npos) {
                result += currentLine.substring(0, lastSpace) + "\n";
                currentLine = currentLine.substring(lastSpace + 1) + charStr;
            } else {
                result += currentLine + "\n";
                currentLine = charStr;
            }
        } else {
            currentLine += charStr;
        }
    }
    result += currentLine;
    m_text->setString(result);
    m_text->setPosition(m_shape.getPosition() + m_padding);
}

void UIBox::setProgressBar(float percentage, sf::Color color) {
    m_hasBar = true;
    m_barPercentage = std::max(0.0f, std::min(1.0f, percentage));
    
    float barMaxWidth = m_shape.getSize().x - (m_padding.x * 2.f);
    m_bar.setSize({barMaxWidth * m_barPercentage, 15.f});
    m_bar.setFillColor(color);
}

void UIBox::render(sf::RenderWindow& window) {
    window.draw(m_shape);
    if (m_hasBar) {
        m_bar.setPosition(m_shape.getPosition() + sf::Vector2f(m_padding.x, m_shape.getSize().y - 25.f));
        window.draw(m_bar);
    }
    if (m_text) {
        window.draw(*m_text);
    }
}

} // namespace solis
