#include "States/EndingState.h"
#include "States/HubState.h"
#include "Core/ResourceManager.h"
#include "Managers/RunManager.h"
#include "Managers/SaveManager.h"
#include <sstream>

namespace solis {

EndingState::EndingState(sf::RenderWindow& window, StateMachine& stateMachine)
    : m_window(window), m_stateMachine(stateMachine) {}

void EndingState::init() {
    sf::Vector2u windowSize = m_window.getSize();
    float w = static_cast<float>(windowSize.x);
    float h = static_cast<float>(windowSize.y);

    m_display = std::make_unique<UIBox>(sf::Vector2f(0.f, 0.f), sf::Vector2f(w, h), sf::Color::Black, sf::Color::White, -2.f);
    m_keyHeld = true;

    calculateEnding();
}

void EndingState::calculateEnding() {
    const SaveData& save = SaveManager::getInstance().getData();
    const sf::Font& font = ResourceManager::getInstance().getFont("main");
    std::stringstream ss;

    ss << "ЭПИЛОГ\n\n";

    if (save.mercy_counter >= 15 && save.blood_counter <= 5 && save.unlocked_lore.size() >= 5) {
        ss << "ИСТИННАЯ КОНЦОВКА: 'Рассвет Сострадания'\n";
        ss << "Вы не позволили тени поглотить вашу душу. Вы нашли слова даже для самых ожесточенных врагов, сохранив знания об ошибках прошлого. Виндикт сложил оружие, признав, что Свет — это не только огонь, но и тепло. Искра вспыхивает с новой силой, освещая подземелья.\n";
    } else if (save.blood_counter >= 20 && save.ideology_score <= -5) {
        ss << "КОНЦОВКА ФИЦИО: 'Выжженная земля'\n";
        ss << "Ваш путь был усеян пеплом. Прагматизм и сила стали вашими главными аргументами. Вы убили Виндикта, забрав его Искру. Теперь вы — новый владыка подземелий, правитель теней, чья жестокость станет новым законом.\n";
    } else if (save.ideology_score >= 5) {
        ss << "КОНЦОВКА ФИНЕСЫ: 'Слепой Альтруизм'\n";
        ss << "Вы старались помочь всем, часто жертвуя собой. Ваши идеалы возвышенны, но в этом суровом мире они привели к истощению. Виндикт ушел в тень, оставив вас замерзать. Свет сохранен, но какой ценой?\n";
    } else {
        ss << "СТАНДАРТНАЯ КОНЦОВКА: 'Бесконечный Цикл'\n";
        ss << "Виндикт повержен, но тьма никуда не ушла. Вы не смогли разгадать тайны этого места и выбрали путь наименьшего сопротивления. Завтра новые стражи займут пустые коридоры, и кому-то придется проделать этот путь заново.\n";
    }

    ss << "\n\n[Нажмите Space, чтобы вернуться в Родовой Очаг]";
    
    std::string finalStr = ss.str();
    m_display->setText(sf::String::fromUtf8(finalStr.begin(), finalStr.end()), font, 26, sf::Color::White);
}

void EndingState::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) m_window.close();

    bool space = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
    
    if (!space) {
        m_keyHeld = false;
        return;
    }
    
    if (m_keyHeld) return;
    m_keyHeld = true;

    // Сброс забега и возврат в Хаб
    RunManager::getInstance().startNewRun();
    m_stateMachine.clearAndSetState(std::make_unique<HubState>(m_window, m_stateMachine));
}

void EndingState::update(float deltaTime) {}

void EndingState::render(sf::RenderWindow& window) {
    m_display->render(window);
}

} // namespace solis
