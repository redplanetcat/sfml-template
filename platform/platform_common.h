#ifndef PLATFORM_COMMON_H

struct shader_file {
  u32 Handle;
  sf::Shader Shader;
  char VertexFilename[PLATFORM_FILENAME_COUNT];
  char FragmentFilename[PLATFORM_FILENAME_COUNT];
};

#define MAX_SHADERS 16
#define MAX_TEXTS 16

struct platform_state {
  sf::RenderWindow Window;

  shader_file Shaders[MAX_SHADERS];
  u32 ShadersCount;

  sf::Text Text[MAX_TEXTS];
  u32 TextCount;
  sf::Font DefaultFont;

  sf::RenderStates RenderStates;
  
  platform_state(u32 Width, u32 Height, const char* Title)
    : Window(sf::VideoMode(Width, Height), Title)
    , ShadersCount(1)
    , TextCount(1)
    , RenderStates(sf::RenderStates::Default) {}
};

platform_state& GetPlatformState() {
  local_persist platform_state State(800, 600, "Apples Game");
  return State;
}



#define PLATFORM_COMMON_H
#endif
