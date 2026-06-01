#ifndef UITEXTLOG_H
#define UITEXTLOG_H

#include "IUIElement.h"
#include "Core/Events.h"
#include "Core/EventBus.h"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <vector>
#include <string>
#include <deque>

class UITextLog : public IUIElement {
public:
    UITextLog(EventBus& bus, const sf::Font& font, sf::FloatRect bounds);
    ~UITextLog() override = default;

    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;
    bool handleInput(const sf::Event& event) override;

    void setPosition(sf::Vector2f pos) override;
    sf::Vector2f getPosition() const override;
    sf::FloatRect getBounds() const override;

private:
    std::string wrapText(const std::string& str, float width) const;
    sf::Color getColor(ColorID id) const;

    struct PendingMessage {
        std::string text;
        sf::Color color;
        size_t charIndex = 0;
    };

    EventBus& m_bus;
    const sf::Font& m_font;
    sf::FloatRect m_bounds;

    std::deque<PendingMessage> m_pendingMessages;
    std::vector<sf::Text> m_history;

    sf::Text m_currentText;
    std::string m_currentLine;

    float m_timer = 0.0f;
    float m_printSpeed = 0.02f;
    float m_scrollOffset = 0.0f;
};

#endif // UITEXTLOG_H