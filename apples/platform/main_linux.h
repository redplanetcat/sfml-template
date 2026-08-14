#ifndef MAIN_LINUX_H
#include "../common/defines.h"
#include "../common/types.h"
#include "../game/game.h"
#include <limits.h>

#define PLATFORM_FILENAME_COUNT PATH_MAX


struct linux_state {
  char BinFilename[PLATFORM_FILENAME_COUNT]; 
  char* BasePathPtr;
};
global linux_state LinuxState;

struct linux_game_code {
  void* GameCodeSO;
  i64 SOLastWriteTime;
  game_update_t* Update;
  game_render_t* Render;
  game_destroy_t* Destroy;
  b32 IsValid;
};

#define MAIN_LINUX_H
#endif
