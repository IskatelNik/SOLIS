#include "Core/StateMachine.h"

namespace solis {

void StateMachine::pushState(std::unique_ptr<State> newState) {
    m_isAdding = true;
    m_newState = std::move(newState);
}

void StateMachine::popState() {
    m_isRemoving = true;
}

void StateMachine::changeState(std::unique_ptr<State> newState) {
    m_isAdding = true;
    m_isReplacing = true;
    m_newState = std::move(newState);
}

void StateMachine::processStateChanges() {
    if (m_isRemoving && !m_states.empty()) {
        m_states.pop();
        m_isRemoving = false;
    }

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

State& StateMachine::getActiveState() const {
    return *m_states.top();
}

bool StateMachine::isEmpty() const {
    return m_states.empty();
}

} // namespace solis
