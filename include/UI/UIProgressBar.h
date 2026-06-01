#ifndef UIPROGRESSBAR_H
#define UIPROGRESSBAR_H

#include "IUIElement.h"
#include "Core/Events.h"
#include "Core/EventBus.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

enum class ProgressBarType { HP, Heat };

class UIProgressBar : public IUIElement {
public:
    UIProgressBar(EventBus& bus, ProgressBarType type, sf::FloatRect bounds, sf::Color bgColor, sf::Color fgColor);
    ~UIProgressBar() override = default;

    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;
    bool handleInput(const sf::Event& event) override;

    void setTargetPercentage(float percent, bool isCritical = false);

    void setPosition(sf::Vector2f pos) override;
    sf::Vector2f getPosition() const override;
    sf::FloatRect getBounds() const override;

private:
    ProgressBarType m_type;
    sf::RectangleShape m_bg;
    sf::RectangleShape m_fg;

    sf::Color m_fgColor;

    float m_maxWidth;
    float m_currentWidth;
    float m_targetWidth;
    float m_lerpSpeed;

    bool m_isCritical;
    float m_timeAccumulator;
};

#endif // UIPROGRESSBAR_H