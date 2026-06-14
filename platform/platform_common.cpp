void
PlatformHandleInput(game_input* Input) {
  sf::Event Event;
  while (GetPlatformState().Window.pollEvent(Event)) {
    if (Event.type == sf::Event::Closed) {
      GetPlatformState().Window.close();
    } else if (Event.type == sf::Event::KeyPressed) {
      Input->Keys[Event.key.code] = true;
    } else if (Event.type == sf::Event::KeyReleased) {
      Input->Keys[Event.key.code] = false;
    } else if (Event.type == sf::Event::MouseButtonPressed) {
      Input->MouseButtons[Event.mouseButton.button] = true; 
    } else if (Event.type == sf::Event::MouseButtonReleased) {
      Input->MouseButtons[Event.mouseButton.button] = false; 
    } else if (Event.type == sf::Event::MouseMoved) {
      sf::Vector2f MousePosition = GetPlatformState().Window.mapPixelToCoords({ Event.mouseMove.x, Event.mouseMove.y });
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
  GetPlatformState().Window.draw(reinterpret_cast<sf::Vertex*>(Vertices), NumVertices, PlatformPrimitiveType, GetPlatformState().RenderStates);
}


//#define PLATFORM_USE_SHADER(name) void name(u32 ShaderHandle);
//typedef PLATFORM_USE_SHADER(platform_use_shader_t);
PLATFORM_USE_SHADER(PlatformUseShader) {
  if (ShaderHandle == 0) {
    GetPlatformState().RenderStates.shader = nullptr;
    return;
  }
  if (GetPlatformState().ShadersCount <= ShaderHandle) {
    return;
  }
  GetPlatformState().RenderStates.shader = &GetPlatformState().Shaders[ShaderHandle].Shader;
}

//#define PLATFORM_SET_SHADER_UNIFORM_VEC2(name) void name(u32 ShaderHandle, char* UniformName, vec2 Vector)
//typedef PLATFORM_SET_SHADER_UNIFORM_VEC2(platform_set_shader_uniform_vec2_t);
PLATFORM_SET_SHADER_UNIFORM_VEC2(PlatformSetShaderUniformVec2) {
  if ((ShaderHandle == 0) || (GetPlatformState().ShadersCount <= ShaderHandle)) {
    return;
  }
  sf::Shader& Shader = GetPlatformState().Shaders[ShaderHandle].Shader;
  Shader.setUniform(UniformName, sf::Vector2f(Vector.x, Vector.y));
}

//#define PLATFORM_CREATE_TEXT(name) u32 name(const char* String, u32 Size, color Color);
//typedef PLATFORM_CREATE_TEXT(platform_create_text_t);
PLATFORM_CREATE_TEXT(PlatformCreateText) {
  if (GetPlatformState().TextCount >= MAX_TEXTS) {
    return 0;
  }
  sf::Text& Text = GetPlatformState().Text[GetPlatformState().TextCount];
  Text.setFont(GetPlatformState().DefaultFont);
  Text.setString(String);
  Text.setCharacterSize(Size);
  Text.setFillColor(sf::Color(Color.r, Color.g, Color.b, Color.a));
  return GetPlatformState().TextCount++;
}

/*#define PLATFORM_UPDATE_TEXT(name) void name(u32 TextHandle, \
                                             const char* String, \
                                             color Color, \
                                             vec2 Position, \
                                             vec2 Scale, \
                                             rect_alignment Alignment)
typedef PLATFORM_UPDATE_TEXT(platform_update_text_t)*/
PLATFORM_UPDATE_TEXT(PlatformUpdateText) {
  if ((TextHandle == 0) || (GetPlatformState().TextCount <= TextHandle)) {
    return;
  }
  sf::Text& Text = GetPlatformState().Text[TextHandle];
  Text.setString(String);
  Text.setFillColor(sf::Color(Color.r, Color.g, Color.b, Color.a));
  Text.setPosition(Position.x, Position.y);
  Text.setScale(sf::Vector2f(Scale.x, Scale.y));
  sf::FloatRect Bounds = Text.getLocalBounds();
  vec2 Origin = { };
  switch (Alignment) {
    case (rect_alignment::TopLeft): {
      Origin = {Bounds.left, Bounds.top};
    }; break;
    case (rect_alignment::TopRight): {
      Origin = {Bounds.left + Bounds.width, Bounds.top};
    }; break;
    case (rect_alignment::Center): {
      Origin = {Bounds.left + Bounds.width / 2.f, Bounds.top + Bounds.height / 2.f};
    }; break;
    case (rect_alignment::BottomLeft): {
      Origin = {Bounds.left, Bounds.top + Bounds.height};
    }; break;
    case (rect_alignment::BottomRight): {
      Origin = {Bounds.left + Bounds.width, Bounds.top + Bounds.height};
    }; break;
  }
  Text.setOrigin(Origin.x, Origin.y);
}


//#define PLATFORM_DRAW_TEXT(name) void name(u32 TextHandle)
//typedef PLATFORM_DRAW_TEXT(platform_draw_text_t);
PLATFORM_DRAW_TEXT(PlatformDrawText) {
  if ((TextHandle == 0) || (GetPlatformState().TextCount <= TextHandle)) {
    return;
  }
  sf::Text& Text = GetPlatformState().Text[TextHandle];
  GetPlatformState().Window.draw(Text);
}

void
AssignPlatformCallbacks(game_memory* Memory) {
  Memory->PlatformCallbacks.PlatformDrawVertices = PlatformDrawVertices;
  Memory->PlatformCallbacks.PlatformLoadShader = PlatformLoadShader;
  Memory->PlatformCallbacks.PlatformUseShader = PlatformUseShader;
  Memory->PlatformCallbacks.PlatformSetShaderUniformVec2 = PlatformSetShaderUniformVec2;
  Memory->PlatformCallbacks.PlatformCreateText = PlatformCreateText;
  Memory->PlatformCallbacks.PlatformUpdateText = PlatformUpdateText;
  Memory->PlatformCallbacks.PlatformDrawText = PlatformDrawText;
}
