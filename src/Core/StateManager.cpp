#include "include/Core/StateManager.h"

StateManager::StateManager(StateFactory factory)
    : m_factory(std::move(factory)),
      m_currentState(nullptr),
      m_pendingState(nullptr) {}

void StateManager::changeState(GameStateID stateId) {
  if (m_factory) {
    m_pendingState = m_factory(stateId);
  }
}

void StateManager::changeState(std::unique_ptr<IState> newState) {
  m_pendingState = std::move(newState);
}

void StateManager::applyPendingChanges() {
  // Если есть состояние в очереди на смену
  if (m_pendingState) {
    // Корректно завершаем текущее состояние, если оно существует
    if (m_currentState) {
      m_currentState->onExit();
    }

    // Передаем владение новым состоянием (старое автоматически удалится)
    m_currentState = std::move(m_pendingState);

    // Инициализируем новое состояние
    m_currentState->onEnter();
  }
}

void StateManager::handleInput(const sf::Event& event) {
  if (m_currentState) {
    m_currentState->handleInput(event);
  }
}

void StateManager::update(float dt) {
  if (m_currentState) {
    m_currentState->update(dt);
  }
}

void StateManager::draw(sf::RenderTarget& target) {
  if (m_currentState) {
    m_currentState->draw(target);
  }
}
