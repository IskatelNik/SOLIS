#include "Core/StateMachine.h"

namespace solis {

/**
 * @brief Добавляет новое состояние в стек поверх текущего.
 */
void StateMachine::pushState(std::unique_ptr<State> newState) {
    m_isAdding = true;
    m_newState = std::move(newState);
}

/**
 * @brief Удаляет текущее активное состояние из стека.
 */
void StateMachine::popState() {
    m_isRemoving = true;
}

/**
 * @brief Заменяет текущее активное состояние на новое.
 */
void StateMachine::changeState(std::unique_ptr<State> newState) {
    m_isAdding = true;
    m_isReplacing = true;
    m_newState = std::move(newState);
}

/**
 * @brief Обрабатывает отложенные изменения состояний.
 * Вызывается один раз в начале каждого кадра.
 */
void StateMachine::processStateChanges() {
    // Удаление состояния
    if (m_isRemoving && !m_states.empty()) {
        m_states.pop();
        // Если после удаления в стеке остались состояния — возобновляем работу верхнего
        if (!m_states.empty() && !m_isAdding) {
            m_states.top()->resume();
        }
        m_isRemoving = false;
    }

    // Добавление или замена состояния
    if (m_isAdding) {
        if (m_isReplacing && !m_states.empty()) {
            m_states.pop();
            m_isReplacing = false;
        }

        m_states.push(std::move(m_newState));
        m_states.top()->init();
        m_isAdding = false;
    }
}

/**
 * @brief Полностью очищает стек и устанавливает новое состояние.
 * Полезно при возврате в главное меню или перезапуске игры.
 */
void StateMachine::clearAndSetState(std::unique_ptr<State> newState) {
    while (!m_states.empty()) {
        m_states.pop();
    }
    m_states.push(std::move(newState));
    m_states.top()->init();
    
    // Сброс всех флагов, так как изменения применены немедленно
    m_isAdding = false;
    m_isRemoving = false;
    m_isReplacing = false;
}

/**
 * @brief Возвращает ссылку на текущее активное состояние.
 */
State& StateMachine::getActiveState() const {
    return *m_states.top();
}

/**
 * @brief Проверяет, пуст ли стек состояний.
 */
bool StateMachine::isEmpty() const {
    return m_states.empty();
}

} // namespace solis
