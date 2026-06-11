#ifndef MAIN_WIN32_H
#include "../common/defines.h"
#include "../common/types.h"
#include "../game/game.h"

struct win32_state {
    char ExeFilename[MAX_PATH];
    char* BasePath;
};

struct win32_game_code {
    HMODULE GameCodeDLL;
    FILETIME DLLLastWriteTime;
    game_update_t* Update;
    game_render_t* Render;
    b32 IsValid;    
};


#define MAIN_WIN32_H
#endif
