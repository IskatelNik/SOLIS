#ifndef EVENTBUS
#define EVENTBUS

#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

class EventBus {
 public:
  // Конструктор и деструктор по умолчанию
  EventBus() = default;
  ~EventBus() = default;

  // Запрещаем копирование и перемещение (Шина должна быть одна и передаваться
  // по ссылке)
  EventBus(const EventBus&) = delete;
  EventBus& operator=(const EventBus&) = delete;
  EventBus(EventBus&&) = delete;
  EventBus& operator=(EventBus&&) = delete;

  /**
   * @brief Подписка на событие определенного типа.
   * @tparam T Тип события (структура из Events.hpp).
   * @param callback Функция или лямбда, которая будет вызвана при публикации
   * события.
   */
  template <typename T>
  void subscribe(std::function<void(const T&)> callback) {
    std::type_index typeIdx(typeid(T));
    m_subscribers[typeIdx].push_back(std::any(callback));
  }

  /**
   * @brief Публикация события (синхронный вызов всех подписчиков).
   * @tparam T Тип события.
   * @param event Экземпляр события с данными.
   */
  template <typename T>
  void publish(const T& event) {
    std::type_index typeIdx(typeid(T));

    auto it = m_subscribers.find(typeIdx);
    if (it != m_subscribers.end()) {
      // Проходим по всем сохраненным коллбекам для данного типа
      for (const auto& anyCallback : it->second) {
        // Извлекаем типизированную функцию из std::any и вызываем её
        auto callback =
            std::any_cast<std::function<void(const T&)>>(anyCallback);
        callback(event);
      }
    }
  }

  /**
   * @brief Полная очистка всех подписок.
   * Полезно при возврате в Главное меню или жестком рестарте.
   */
  void clear() { m_subscribers.clear(); }

 private:
  // Хранилище обработчиков.
  // Ключ - тип события (std::type_index)
  // Значение - массив коллбеков, спрятанных в std::any
  std::unordered_map<std::type_index, std::vector<std::any>> m_subscribers;
};

#endif  // !EVENTBUS
