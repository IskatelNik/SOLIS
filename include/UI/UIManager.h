#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "UI/IUIElement.h"
#include <vector>
#include <memory>
#include <utility>

/**
 * @brief Владеет всеми UI элементами текущего стейта.
 * Централизованно обновляет, рисует и передает им события.
 */
class UIManager {
public:
    UIManager() = default;
    ~UIManager() = default;

    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    /**
     * @brief Шаблонная фабрика.
     * Создает элемент, берет его в уникальное владение и возвращает сырой указатель наружу для настройки.
     */
    template <typename T, typename... Args>
    T* addElement(Args&&... args) {
        auto element = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = element.get();
        m_elements.push_back(std::move(element));
        return ptr;
    }

    void clear();

    void updateAll(float dt);
    void drawAll(sf::RenderTarget& target) const;
    bool handleInputAll(const sf::Event& event);

private:
    std::vector<std::unique_ptr<IUIElement>> m_elements;
};

#endif // UIMANAGER_H