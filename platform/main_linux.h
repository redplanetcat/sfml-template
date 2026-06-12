#ifndef MAIN_LINUX_H
#include "../common/defines.h"
#include "../common/types.h"
#include "../game/game.h"
#include <limits.h>

struct linux_state {
  char BinFilename[PATH_MAX]; 
  char* BasePath;
};

struct linux_game_code {
  void* GameCodeSO;
  i64 SOLastWriteTime;
  game_update_t* Update;
  game_render_t* Render;
  b32 IsValid;
};

#define MAIN_LINUX_H
#endif
