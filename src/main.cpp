#include <SFML/Graphics.hpp>
#include <cmath>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define RAD2DEG(a) (a * 180 / M_PI) 

int main(int _argc, char** _argv) {
  sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Template");
  window.setFramerateLimit(60);

  sf::CircleShape triangle = sf::CircleShape{ 50.f, 3u };
  triangle.setFillColor({ 100u, 50u, 200u });
  triangle.setOrigin({ std::round(triangle.getLocalBounds().width / 2.f), std::round(triangle.getLocalBounds().height / 2.f) });
  triangle.setPosition({ WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f });

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::MouseMoved) {
        auto mouse_position = window.mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y });
        auto delta_position = mouse_position - triangle.getPosition();
        auto rotation = RAD2DEG(std::atan2(delta_position.y, delta_position.x)) + 90.f;

        triangle.setRotation(rotation);
      }

    }

    window.clear();
    window.draw(triangle);
    window.display();
  }
  return 0;
}

