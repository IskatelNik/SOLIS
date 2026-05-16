#include <SFML/Graphics.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <Windows.h>

int main() {

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
  // В SFML 3 VideoMode принимает sf::Vector2u через фигурные скобки
  sf::RenderWindow window(sf::VideoMode({1920, 1080}), "SOLIS - Test Build");

  // Проверка работы JSON
  nlohmann::json testJson;
  testJson["hero"] = "Ignis";
  std::cout << "JSON Test: " << testJson.dump() << std::endl;

  while (window.isOpen()) {
    // В SFML 3 pollEvent возвращает std::optional
    while (const std::optional event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }

    window.clear(sf::Color::Black);
    window.display();
  }
  return 0;
}
