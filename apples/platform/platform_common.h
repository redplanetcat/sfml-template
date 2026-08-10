#ifndef PLATFORM_COMMON_H

struct texture_file {
  sf::Texture Texture;
  char Filename[PLATFORM_FILENAME_COUNT];
};

struct shader_file {
  sf::Shader Shader;
  char VertexFilename[PLATFORM_FILENAME_COUNT];
  char FragmentFilename[PLATFORM_FILENAME_COUNT];
};

struct sound_file {
  sf::SoundBuffer Buffer;
  sf::Sound Sound;
  char Filename[PLATFORM_FILENAME_COUNT];
};

#define MAX_TEXTURES 16
#define MAX_SHADERS 16
#define MAX_SOUNDS 16
#define MAX_TEXTS 16

struct platform_state {
  sf::RenderWindow Window;

  texture_file Textures[MAX_TEXTURES];
  u32 TexturesCount;

  shader_file Shaders[MAX_SHADERS];
  u32 ShadersCount;

  sound_file Sounds[MAX_SOUNDS];
  u32 SoundsCount;

  sf::Text Text[MAX_TEXTS];
  u32 TextCount;
  sf::Font DefaultFont;

  sf::RenderStates RenderStates;
  
  platform_state(u32 Width, u32 Height, const char* Title)
    : Window(sf::VideoMode(Width, Height), Title)
    , TexturesCount(1)
    , ShadersCount(1)
    , SoundsCount(1)
    , TextCount(1)
    , RenderStates(sf::RenderStates::Default) {}
};

platform_state& GetPlatformState() {
  local_persist platform_state State(800, 600, "Apples Game");
  return State;
}

#define PLATFORM_COMMON_H
#endif
