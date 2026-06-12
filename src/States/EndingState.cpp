#include "States/EndingState.h"
#include "States/HubState.h"
#include "Core/ResourceManager.h"
#include "Managers/RunManager.h"
#include "Managers/SaveManager.h"
#include <sstream>

namespace solis {

/**
 * Состояние финала игры. Выводит эпилог на основе действий игрока в забегах.
 */
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

/**
 * Определяет, какой финал заслужил игрок, основываясь на счетчиках крови/милосердия и идеологии.
 */
void EndingState::calculateEnding() {
    const SaveData& save = SaveManager::getInstance().getData();
    const sf::Font& font = ResourceManager::getInstance().getFont("main");
    std::stringstream ss;

    int total_actions = save.mercy_counter + save.blood_counter;
    double mercy_percent = (total_actions > 0) ? (static_cast<double>(save.mercy_counter) / total_actions) * 100.0 : 0.0; 

    ss << "ЭПИЛОГ\n\n";

    // Условие Истинного Финала: Милосердие + Все Знания + Положительная идеология
    if (total_actions >= 10 && mercy_percent >= 80.0 && save.unlocked_lore.size() >= 80 && save.ideology_score > 0) {
        ss << "Новый Рассвет над новой Империей\n";
        ss << "Даже когда было невыносимо жарко, ты выбирал милосердие. Ты растопил пламя в сердце общества. Огонь — это оружие и тепло, жгучая боль и очаг. Небеса снова светлеют. Солюмны и люди складывают оружие. Круг ненависти разорван. Солис начинает гореть с новой силой, освещая некогда разрозненную Империю. Твое сердце наполняется уверенностью... и надеждой.\n";
    } 
    // Финал Разрушителя: Жестокость + Отрицательная идеология
    else if (total_actions >= 10 && mercy_percent <= 20.0 && save.ideology_score < 0) {
        ss << "И поглатит всё беспощадное пламя\n";
        ss << "Пыль и пепел. Вот и всё, что осталось на твоем пути. Ты забрал то пламя, что горело в сердцах людей, присвоив его себе. Ты доказал, что в этом мире имеют значение только сильнейшие. Третий Цезарь пал. Больше никто и никогда не посмеет причинить тебе боль. Ты победил. Но почему-то... здесь стало еще холоднее.\n";
    } 
    // Нейтрально-положительный финал
    else if (save.ideology_score >= 5) {
        ss << "Ибо Солнце еще не потухло'\n";
        ss << "Ты видел свет в сердцах каждого. Ты хотел, чтобы никто больше не страдал. И ради этого ты отдал всё, что у тебя было. Своё пламя. Свой Свет. Солис на небе снова сияет, согревая Империю. Никто не знает, надолго ли загорелось светило вновь. Но тебе уже поздно об этом думать... засыпай, ты сделал всё что мог.\n";
    } 
    // Стандартный финал (побег в никуда)
    else {
        ss << "Как свеча, Затухает Солнце'\n";
        ss << "Ты прошел через мириаду испытаний, чтобы выйти на поверхность. Пламя внутри тебя не смогло зажечь Империю вновь. Может быть, эту землю спасёт настоящий герой, но это уже будешь не ты... ты будешь никому неизвестным беглецом, странником. Иди дальше, ведь это всё что ты теперь можешь...\n";
    }

    ss << "\n\n[Нажмите Space, чтобы вернуться в Родовой Очаг]";
    m_display->setText(sf::String::fromUtf8(ss.str().begin(), ss.str().end()), font, 26, sf::Color::White);
}

void EndingState::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) m_window.close();

    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        m_keyHeld = false;
        return;
    }
    
    if (m_keyHeld) return;
    m_keyHeld = true;

    // Сброс забега и возврат в Хаб для новой попытки
    RunManager::getInstance().startNewRun();
    m_stateMachine.clearAndSetState(std::make_unique<HubState>(m_window, m_stateMachine));
}

void EndingState::update(float deltaTime) {}

void EndingState::render(sf::RenderWindow& window) {
    m_display->render(window);
}

} // namespace solis
