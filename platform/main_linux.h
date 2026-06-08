#ifndef MAIN_LINUX_H
#include "../common/defines.h"
#include "../common/types.h"
#include "../game/game.h"

struct linux_game_code {
  void* GameCodeSO;
  i64 SOLastWriteTime;
  game_update_t* GameUpdate;
  game_render_t* GameRender;
  b32 IsValid;
};

#define MAIN_LINUX_H
#endif
