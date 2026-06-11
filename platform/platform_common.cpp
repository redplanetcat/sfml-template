sf::CircleShape triangle;
sf::RenderWindow PlatformWindow;
sf::Event PlatformEvent;

PLATFORM_HANDLE_INPUT(PlatformHandleInput) {
  while (PlatformWindow.pollEvent(PlatformEvent)) {
    if (PlatformEvent.type == sf::Event::Closed) {
      PlatformWindow.close();
    } else if (PlatformEvent.type == sf::Event::MouseMoved) {
      auto mouse_position = PlatformWindow.mapPixelToCoords({ PlatformEvent.mouseMove.x, PlatformEvent.mouseMove.y });
      auto delta_position = mouse_position - triangle.getPosition();
      float rotation = RAD2DEG((float)std::atan2(delta_position.y, delta_position.x)) + 90.f;

      triangle.setRotation(rotation);
    }
  }
}

PLATFORM_RENDER_TRIANGLE(PlatformRenderTriangle) {
  PlatformWindow.draw(triangle);
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
  Memory->PlatformCallbacks.PlatformHandleInput = PlatformHandleInput;
  Memory->PlatformCallbacks.PlatformRenderTriangle = PlatformRenderTriangle;
  Memory->PlatformCallbacks.PlatformDrawVertices = PlatformDrawVertices;
}
