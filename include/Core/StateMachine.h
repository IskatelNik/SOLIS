#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "Core/State.h"
#include <memory>
#include <stack>

namespace solis {

class StateMachine {
public:
    StateMachine() = default;
    ~StateMachine() = default;

    void pushState(std::unique_ptr<State> newState);
    void popState();
    void changeState(std::unique_ptr<State> newState);

    void processStateChanges();

    void clearAndSetState(std::unique_ptr<State> newState); // MVP 4 Fix: For Game Over

    State& getActiveState() const;
    bool isEmpty() const;

private:
    std::stack<std::unique_ptr<State>> m_states;
    std::unique_ptr<State> m_newState;

    bool m_isRemoving = false;
    bool m_isAdding = false;
    bool m_isReplacing = false;
};

} // namespace solis

#endif // STATE_MACHINE_H
