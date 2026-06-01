#include "UI/UIProgressBar.h"
#include <cmath>

UIProgressBar::UIProgressBar(EventBus& bus, ProgressBarType type, sf::FloatRect bounds, sf::Color bgColor, sf::Color fgColor)
    : m_type(type), m_fgColor(fgColor), m_maxWidth(bounds.size.x), m_currentWidth(bounds.size.x),
    m_targetWidth(bounds.size.x), m_lerpSpeed(5.0f), m_isCritical(false), m_timeAccumulator(0.0f)
{
    m_bg.setPosition(bounds.position);
    m_bg.setSize(bounds.size);
    m_bg.setFillColor(bgColor);

    m_fg.setPosition(bounds.position);
    m_fg.setSize(bounds.size);
    m_fg.setFillColor(fgColor);

    // Изолированная подписка в зависимости от типа полоски
    if (m_type == ProgressBarType::HP) {
        bus.subscribe<Evt_PlayerHPChanged>([this](const Evt_PlayerHPChanged& e) {
            float percent = (e.maxHP > 0) ? static_cast<float>(e.currentHP) / e.maxHP : 0.0f;
            setTargetPercentage(percent, percent <= 0.2f); // Критическое ХП < 20%
            });
    }
    else if (m_type == ProgressBarType::Heat) {
        bus.subscribe<Evt_PlayerHeatChanged>([this](const Evt_PlayerHeatChanged& e) {
            float percent = (e.maxHeat > 0) ? static_cast<float>(e.currentHeat) / e.maxHeat : 0.0f;
            setTargetPercentage(percent, e.isOverheated || percent >= 0.85f);
            });
    }
}

void UIProgressBar::setTargetPercentage(float percent, bool isCritical) {
    if (percent < 0.0f) percent = 0.0f;
    if (percent > 1.0f) percent = 1.0f;

    m_targetWidth = m_maxWidth * percent;
    m_isCritical = isCritical;
}

void UIProgressBar::update(float dt) {
    // Вручную реализуем lerp для ширины
    if (std::abs(m_targetWidth - m_currentWidth) > 0.5f) {
        m_currentWidth = m_currentWidth + (m_targetWidth - m_currentWidth) * (m_lerpSpeed * dt);
    }
    else {
        m_currentWidth = m_targetWidth;
    }

    m_fg.setSize(sf::Vector2f({ m_currentWidth, m_bg.getSize().y }));

    // Пульсация при критическом состоянии
    if (m_isCritical) {
        m_timeAccumulator += dt;
        auto alpha = static_cast<std::uint8_t>(155 + 100 * std::sin(m_timeAccumulator * 8.0f)); // мигание
        sf::Color pulseColor = m_fgColor;
        pulseColor.a = alpha;
        m_fg.setFillColor(pulseColor);
    }
    else {
        m_fg.setFillColor(m_fgColor);
    }
}

void UIProgressBar::draw(sf::RenderTarget& target) const {
    target.draw(m_bg);
    target.draw(m_fg);
}

bool UIProgressBar::handleInput(const sf::Event& /*event*/) {
    return false; // Полоска прогресса не интерактивна для мыши
}

void UIProgressBar::setPosition(sf::Vector2f pos) {
    m_bg.setPosition(pos);
    m_fg.setPosition(pos);
}

sf::Vector2f UIProgressBar::getPosition() const { return m_bg.getPosition(); }
sf::FloatRect UIProgressBar::getBounds() const { return m_bg.getGlobalBounds(); }