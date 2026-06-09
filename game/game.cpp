#include "game.h"

internal u64
CreateEntity() {

}

internal u64
DeleteEntity(u64 ID) {

}

internal void
GameReset(game_state* GameState) {
  game_state->Entities = entity_manager();
}

internal void 
GameInit(game_state* GameState, platform_callbacks* Callbacks) {
  game_state->Entities = entity_manager();
}

#if defined __cplusplus
extern "C"
#endif
GAME_UPDATE(GameUpdate) {
  game_state* GameState = (game_state*)Memory->PermanentStorage;
  if (!GameState->IsInitialized) {
    GameInit(GameState, &Memory->PlatformCallbacks);
    GameState->IsInitialized = true;
  }
  Memory->PlatformCallbacks.PlatformHandleInput();
  vertex Vertices[6] = {
    {{ 0.f, 0.f }, { 0, 0, 255, 255 }, { -1.f, 0.f }},
    {{ 255.f, 0.f }, { 0, 255, 0, 255 }, { -1.f, 0.f }},
    {{ 255.f, 255.f }, { 255, 0, 0, 255 }, { -1.f, 0.f }},
    {{ 0.f, 0.f }, { 0, 0, 255, 255 }, { -1.f, 0.f }},
    {{ 0.f, 255.f }, { 255, 0, 255, 255 }, { -1.f, 0.f }},
    {{ 255.f, 255.f }, { 255, 0, 0, 255 }, { -1.f, 0.f }},
  };
  Memory->PlatformCallbacks.PlatformPushVertices(Vertices, 6);
}

#if defined __cplusplus
extern "C"
#endif
GAME_RENDER(GameRender) {
  if (!Memory->IsInitialized) {

    Memory->IsInitialized = true;
  }
  game_state* GameState = (game_state*)Memory->PermanentStorage;
  Memory->PlatformCallbacks.PlatformRenderTriangle();
  Memory->PlatformCallbacks.PlatformDrawVertices(PRIMITIVE_TRIANGLES);
}
