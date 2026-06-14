#ifndef PLATFORM_COMMON_H

struct shader_file {
  u32 Handle;
  sf::Shader Shader;
  char VertexFilename[PLATFORM_FILENAME_COUNT];
  char FragmentFilename[PLATFORM_FILENAME_COUNT];
};

sf::RenderWindow PlatformWindow;
sf::Event PlatformEvent;

#define MAX_SHADERS 16
global shader_file PlatformShaders[MAX_SHADERS];
global u32 PlatformShadersCount = 1;

#define MAX_TEXTS 16
global sf::Text PlatformText[MAX_TEXTS];
global u32 PlatformTextCount = 1;
global sf::Font PlatformDefaultFont;

global sf::RenderStates PlatformRenderStates = sf::RenderStates::Default;


#define PLATFORM_COMMON_H
#endif
