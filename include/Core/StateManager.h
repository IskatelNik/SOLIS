#ifndef STATEMANAGER
#define

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <functional>
#include <memory>

#include "Events.h"  // Подключаем для GameStateID
#include "IState.h"

class StateManager {
 public:
  // Фабрика для отложенного создания стейтов без инклуда всех экранов игры
  using StateFactory = std::function<std::unique_ptr<IState>(GameStateID)>;

  explicit StateManager(StateFactory factory = nullptr);
  ~StateManager() = default;

  // Планирование смены состояния по его ID (используя фабрику)
  void changeState(GameStateID stateId);

  // Планирование смены состояния с прямой передачей владеющего указателя
  void changeState(std::unique_ptr<IState> newState);

  // Безопасное применение изменений (вызывается в main-цикле вне update/draw)
  void applyPendingChanges();

  // Прокси-методы для текущего состояния
  void handleInput(const sf::Event& event);
  void update(float dt);
  void draw(sf::RenderTarget& target);

 private:
  StateFactory m_factory;

  std::unique_ptr<IState> m_currentState;
  std::unique_ptr<IState> m_pendingState;
};

#endif  // !STATEMANAGER
