#include "UI/UITextLog.h"
#include <cmath>

UITextLog::UITextLog(EventBus& bus, const sf::Font& font, sf::FloatRect bounds)
    : m_bus(bus), m_font(font), m_bounds(bounds), m_currentText(font)
{
    m_currentText.setCharacterSize(20);

    m_bus.subscribe<Evt_LogMessage>([this](const Evt_LogMessage& e) {
        PendingMessage msg;
        msg.text = wrapText(e.text, m_bounds.size.x);
        msg.color = getColor(e.color);
        m_pendingMessages.push_back(msg);
        });

    m_bus.subscribe<Evt_ClearLog>([this](const Evt_ClearLog&) {
        m_history.clear();
        m_pendingMessages.clear();
        m_currentLine.clear();
        m_currentText.setString("");
        m_scrollOffset = 0.0f;
        });
}

void UITextLog::update(float dt) {
    if (m_pendingMessages.empty()) {
        return;
    }

    m_timer += dt;
    if (m_timer >= m_printSpeed) {
        m_timer = 0.0f;
        auto& msg = m_pendingMessages.front();

        // Устанавливаем цвет для текущего печатаемого сообщения
        m_currentText.setFillColor(msg.color);

        if (msg.charIndex < msg.text.size()) {
            m_currentLine += msg.text[msg.charIndex];
            msg.charIndex++;
            m_currentText.setString(m_currentLine);
        }
        else {
            // Сообщение завершено, переносим в историю
            m_history.push_back(m_currentText);
            m_currentLine.clear();
            m_currentText.setString("");
            m_pendingMessages.pop_front();
        }
    }
}

void UITextLog::draw(sf::RenderTarget& target) const {
    // Отрисовка снизу вверх, чтобы новые сообщения выталкивали старые
    float currentY = std::round(m_bounds.position.y + m_bounds.size.y - 30.0f + m_scrollOffset);
    float startX = std::round(m_bounds.position.x);

    // Сначала рисуем то, что сейчас печатается (в самом низу)
    if (!m_currentLine.empty()) {
        sf::Text renderText = m_currentText;
        renderText.setPosition(sf::Vector2f({ startX, currentY }));
        target.draw(renderText);
        currentY -= std::round(renderText.getLocalBounds().size.y + 10.0f);
    }

    // Затем рисуем историю в обратном порядке
    for (auto it = m_history.rbegin(); it != m_history.rend(); ++it) {
        sf::Text histText = *it;
        histText.setPosition(sf::Vector2f({ startX, currentY }));

        // Отрисовываем только если текст внутри границ по Y (верхняя граница)
        if (currentY + histText.getLocalBounds().size.y > m_bounds.position.y) {
            target.draw(histText);
        }

        currentY -= std::round(histText.getLocalBounds().size.y + 10.0f);
        if (currentY < m_bounds.position.y - 50.0f) {
            break; // Экономим ресурсы, не рисуем то, что ушло далеко за край
        }
    }
}

bool UITextLog::handleInput(const sf::Event& event) {
    if (const auto* scroll = event.getIf<sf::Event::MouseWheelScrolled>()) {
        m_scrollOffset += scroll->delta * 15.0f;
        if (m_scrollOffset < 0.0f) m_scrollOffset = 0.0f; // Блокируем скролл ниже актуального текста
        return true;
    }
    return false;
}

void UITextLog::setPosition(sf::Vector2f pos) { m_bounds.position = pos; }
sf::Vector2f UITextLog::getPosition() const { return m_bounds.position; }
sf::FloatRect UITextLog::getBounds() const { return m_bounds; }

std::string UITextLog::wrapText(const std::string& str, float width) const {
    std::string result;
    std::string currentLine;
    sf::Text text(m_font);
    text.setCharacterSize(20);

    size_t start = 0;
    while (start < str.length()) {
        size_t spaceIdx = str.find(' ', start);
        if (spaceIdx == std::string::npos) spaceIdx = str.length();

        std::string word = str.substr(start, spaceIdx - start);
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;

        text.setString(testLine);
        if (text.getLocalBounds().size.x > width && !currentLine.empty()) {
            result += currentLine + "\n";
            currentLine = word;
        }
        else {
            currentLine = testLine;
        }
        start = spaceIdx + 1;
    }
    result += currentLine;
    return result;
}

sf::Color UITextLog::getColor(ColorID id) const {
    switch (id) {
    case ColorID::Red:    return sf::Color(220, 50, 50);
    case ColorID::Green:  return sf::Color(50, 220, 50);
    case ColorID::Yellow: return sf::Color(255, 200, 50);
    case ColorID::Blue:   return sf::Color(50, 150, 255);
    case ColorID::Default:
    default:              return sf::Color(220, 220, 220); // Базовый цвет текста (светло-серый)
    }
}