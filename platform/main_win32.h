#ifndef MAIN_WIN32_H
#include "../common/defines.h"
#include "../common/types.h"
#include "../game/game.h"

FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }

#define PLATFORM_FILENAME_COUNT MAX_PATH

struct win32_shader_file {
  u32 Handle;
  sf::Shader Shader;
  char VertexFilename[PLATFORM_FILENAME_COUNT];
  char FragmentFilename[PLATFORM_FILENAME_COUNT];
};

struct win32_state {
    char ExeFilename[PLATFORM_FILENAME_COUNT];
    char* BasePathPtr;
};
global win32_state Win32State;

struct win32_game_code {
    HMODULE GameCodeDLL;
    FILETIME DLLLastWriteTime;
    game_update_t* Update;
    game_render_t* Render;
    b32 IsValid;
};


#define MAIN_WIN32_H
#endif
