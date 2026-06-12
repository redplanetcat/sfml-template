sf::CircleShape triangle;
sf::RenderWindow PlatformWindow;
sf::Event PlatformEvent;

void
PlatformHandleInput(game_input* Input) {
  while (PlatformWindow.pollEvent(PlatformEvent)) {
    if (PlatformEvent.type == sf::Event::Closed) {
      PlatformWindow.close();
    } else if (PlatformEvent.type == sf::Event::KeyPressed) {
      Input->Keys[PlatformEvent.key.code] = true;
    } else if (PlatformEvent.type == sf::Event::KeyReleased) {
      Input->Keys[PlatformEvent.key.code] = false;
    } else if (PlatformEvent.type == sf::Event::MouseButtonPressed) {
      Input->MouseButtons[PlatformEvent.mouseButton.button] = true; 
    } else if (PlatformEvent.type == sf::Event::MouseButtonReleased) {
      Input->MouseButtons[PlatformEvent.mouseButton.button] = false; 
    } else if (PlatformEvent.type == sf::Event::MouseMoved) {
      sf::Vector2f MousePosition = PlatformWindow.mapPixelToCoords({ PlatformEvent.mouseMove.x, PlatformEvent.mouseMove.y });
      vec2 PreviousMousePosition = Input->MousePosition;
      Input->MousePosition = { MousePosition.x, MousePosition.y };
      Input->MouseDelta = { MousePosition.x - PreviousMousePosition.x, 
                                MousePosition.y - PreviousMousePosition.y };
    }
  }
}

PLATFORM_DRAW_VERTICES(PlatformDrawVertices) {
  sf::PrimitiveType PlatformPrimitiveType;
  switch (PrimitiveType) {
    case primitive_type::Points: {
      PlatformPrimitiveType = sf::PrimitiveType::Points;
    }; break;
    case primitive_type::Lines: {
      PlatformPrimitiveType = sf::PrimitiveType::Lines;
    }; break;
    case primitive_type::Triangles:
    default: {
      PlatformPrimitiveType = sf::PrimitiveType::Triangles;
    };
  }
  PlatformWindow.draw(reinterpret_cast<sf::Vertex*>(Vertices), NumVertices, PlatformPrimitiveType);
}

void
AssignPlatformCallbacks(game_memory* Memory) {
  Memory->PlatformCallbacks.PlatformDrawVertices = PlatformDrawVertices;
}
