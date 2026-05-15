#ifndef IState
#define

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

// Предварительное объявление классов для избежания циклических зависимостей
class StateManager;
class EventBus;

class IState {
 protected:
  StateManager& m_stateManager;
  EventBus& m_eventBus;

 public:
  // Конструктор базового класса обязывает всех наследников принимать эти
  // зависимости
  IState(StateManager& stateManager, EventBus& eventBus)
      : m_stateManager(stateManager), m_eventBus(eventBus) {}

  // Виртуальный деструктор обязателен для полиморфного удаления
  virtual ~IState() = default;

  // Жизненный цикл
  virtual void onEnter() = 0;
  virtual void onExit() = 0;

  // Игровой цикл
  // В SFML 3 сюда передается уже распакованный (полученный из std::optional)
  // event
  virtual void handleInput(const sf::Event& event) = 0;
  virtual void update(float dt) = 0;

  // Отрисовка происходит в абстрактный RenderTarget для поддержки шейдеров и
  // RenderTexture
  virtual void draw(sf::RenderTarget& target) = 0;
};

#endif  // !IState
