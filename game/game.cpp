#include "game.h"

#if defined __cplusplus
extern "C"
#endif
GAME_UPDATE(GameUpdate){
  game_state* GameState = (game_state*)Memory->PermanentStorage;
  Memory->PlatformHandleInput();
}

#if defined __cplusplus
extern "C"
#endif
GAME_RENDER(GameRender) {
  if (!Memory->IsInitialized) {

    Memory->IsInitialized = true;
  }
  game_state* GameState = (game_state*)Memory->PermanentStorage;
  Memory->PlatformRenderTriangle();
}
